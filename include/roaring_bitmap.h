#ifndef ROARING_BITMAP_H
#define ROARING_BITMAP_H

#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <roaring/roaring.h>

/**
 * C++ wrapper for the CRoaring bitmap library.
 * Provides efficient compressed bitmap functionality for sparse integer sets.
 */
class RoaringBitmap {
public:
    // Constructors and destructor
    RoaringBitmap();
    RoaringBitmap(const RoaringBitmap& other);
    RoaringBitmap(RoaringBitmap&& other) noexcept;
    ~RoaringBitmap();

    // Assignment operators
    RoaringBitmap& operator=(const RoaringBitmap& other);
    RoaringBitmap& operator=(RoaringBitmap&& other) noexcept;

    // Element operations
    void add(uint32_t value);
    void remove(uint32_t value);
    bool contains(uint32_t value) const;
    uint64_t cardinality() const;
    bool isEmpty() const;

    // Bitmap operations (returning new instance)
    RoaringBitmap operator&(const RoaringBitmap& other) const; // AND
    RoaringBitmap operator|(const RoaringBitmap& other) const; // OR
    RoaringBitmap operator-(const RoaringBitmap& other) const; // ANDNOT
    RoaringBitmap operator^(const RoaringBitmap& other) const; // XOR

    // Bitmap operations (in-place)
    RoaringBitmap& operator&=(const RoaringBitmap& other); // AND
    RoaringBitmap& operator|=(const RoaringBitmap& other); // OR
    RoaringBitmap& operator-=(const RoaringBitmap& other); // ANDNOT
    RoaringBitmap& operator^=(const RoaringBitmap& other); // XOR

    // Iterator class for traversing bitmap elements
    class Iterator {
    public:
        Iterator(const RoaringBitmap& bitmap, bool end = false);
        uint32_t operator*() const;
        Iterator& operator++();
        bool operator!=(const Iterator& other) const;
    private:
        const RoaringBitmap& bitmap_;
        uint32_t current_value_;
        uint64_t current_index_;
        uint64_t cardinality_;
    };

    // Range-based for loop support
    Iterator begin() const;
    Iterator end() const;

    // Access to underlying C implementation
    roaring_bitmap_t* getRawBitmap() const;

private:
    // Custom deleter for roaring_bitmap_t
    struct RoaringDeleter {
        void operator()(roaring_bitmap_t* bitmap) const {
            if (bitmap) roaring_bitmap_free(bitmap);
        }
    };
    
    // Smart pointer to underlying bitmap
    std::unique_ptr<roaring_bitmap_t, RoaringDeleter> bitmap_;
};

#endif // ROARING_BITMAP_H