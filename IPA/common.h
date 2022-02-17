#ifndef TYPES_H
#define TYPES_H
#include <vector>


typedef signed char   i8;
typedef unsigned char u8;

typedef signed short   i16;
typedef unsigned short u16;

typedef signed int   i32;
typedef unsigned int u32;

typedef signed long long   i64;
typedef unsigned long long u64;

typedef float  f32;
typedef double f64;


static_assert(sizeof(i8) == 1, "Weird size of i8");
static_assert(sizeof(u8) == 1, "Weird size of u8");

static_assert(sizeof(i16) == 2, "Weird size of i16");
static_assert(sizeof(u16) == 2, "Weird size of u16");

static_assert(sizeof(i32) == 4, "Weird size of i32");
static_assert(sizeof(u32) == 4, "Weird size of u32");

static_assert(sizeof(i64) == 8, "Weird size of i64");
static_assert(sizeof(u64) == 8, "Weird size of u64");

static_assert(sizeof(f32) == 4, "Weird size of f32");
static_assert(sizeof(f64) == 8, "Weird size of f64");



#endif // TYPES_H