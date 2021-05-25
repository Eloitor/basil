#ifndef BASIL_OPTIONAL_H
#define BASIL_OPTIONAL_H

#include "defs.h"
#include "io.h"
#include "rc.h"

template<typename T>
struct optional {
    u8 data[sizeof(T)];
    bool present;

    optional(): present(false) {}
    
    template<typename ...Args>
    static optional some(Args... args) {
        optional o;
        new(o.data) T(args...);
        o.present = true;
        return o;
    }

    static optional none() {
        return optional();
    }

    ~optional() {
        if (present) (*(T*)data).~T();
    }

    optional(const optional& other): present(other.present) {
        if (present) new(data) T(*(T*)other.data);
    }

    optional& operator=(const optional& other) {
        if (this != &other) {
            if (present) (*(T*)data).~T();
            present = other.present;
            if (present) new(data) T(*(T*)other.data);
        }
        return *this;
    }

    operator bool() const {
        return present;
    }

    const T& operator*() const {
        if (!present) panic("Attempted to dereference empty optional!");
        return *(const T*)data;
    }

    T& operator*() {
        if (!present) panic("Attempted to dereference empty optional!");
        return *(T*)data;
    }

    const T* operator->() const {
        if (!present) panic("Attempted to dereference empty optional!");
        return (const T*)data;
    }

    T* operator->() {
        if (!present) panic("Attempted to dereference empty optional!");
        return (T*)data;
    }
};

template<typename T>
struct optional<rc<T>> {
    rc<T> data;

    optional(): data(nullptr) {}
    optional(rc<T> r): data(r) {}

    static optional<rc<T>> some(rc<T> r) {
        optional<rc<T>> o;
        o.data = r;
        return o;
    }


    template<typename ...Args>
    static optional<rc<T>> some(Args... args) {
        optional<rc<T>> o;
        o.data = ref<T>(args...);
        return o;
    }

    static optional<rc<T>> none() {
        return optional<rc<T>>();
    }

    operator bool() const {
        return data;
    }

    const T& operator*() const {
        if (!data) panic("Attempted to dereference empty optional!");
        return *data;
    }

    T& operator*() {
        if (!data) panic("Attempted to dereference empty optional!");
        return *data;
    }

    const T* operator->() const {
        if (!data) panic("Attempted to dereference empty optional!");
        return data.operator->();
    }

    T* operator->() {
        if (!data) panic("Attempted to dereference empty optional!");
        return data.operator->();
    }
};

template<typename T>
struct optional<T&> {
    T* data;

    optional(): data(nullptr) {}
    optional(T* r): data(r) {}

    static optional<T&> none() {
        return optional<T&>();
    }

    static optional<T&> some(T& t) {
        return optional<T&>(&t);
    }

    operator bool() const {
        return data;
    }

    const T& operator*() const {
        if (!data) panic("Attempted to dereference empty optional!");
        return *data;
    }

    T& operator*() {
        if (!data) panic("Attempted to dereference empty optional!");
        return *data;
    }

    const T* operator->() const {
        if (!data) panic("Attempted to dereference empty optional!");
        return data;
    }

    T* operator->() {
        if (!data) panic("Attempted to dereference empty optional!");
        return data;
    }

    operator optional<const T&>() const {
        return optional<const T&>(data);
    }
};

template<typename T>
struct optional<const T&> {
    const T* data;

    optional(): data(nullptr) {}
    optional(const T* r): data(r) {}

    static optional<const T&> none() {
        return optional<const T&>();
    }

    static optional<const T&> some(const T& t) {
        return optional<const T&>(&t);
    }

    operator bool() const {
        return data;
    }

    const T& operator*() const {
        if (!data) panic("Attempted to dereference empty optional!");
        return *data;
    }

    const T* operator->() const {
        if (!data) panic("Attempted to dereference empty optional!");
        return data;
    }
};

template<typename T, typename ...Args>
optional<T> some(Args&&... args) {
    return optional<T>::some(args...);
}

template<typename T>
optional<T> none() {
    return optional<T>::none();
}

template<typename T, typename F>
optional<T> apply(const optional<T>& o, const F& func) {
    if (o.present) return some<T>(func(*o));
    else return o;
}

template<typename T>
void write(stream& io, const optional<T>& o) {
    if (o) write(io, "Some(", *o, ")");
    else write(io, "None");
}

#endif