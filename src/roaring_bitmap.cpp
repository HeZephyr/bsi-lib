#include "roaring_bitmap.h"

RoaringBitmap::RoaringBitmap() 
    : bitmap_(roaring_bitmap_create()) {}

RoaringBitmap::RoaringBitmap(const RoaringBitmap& other)
    : bitmap_(roaring_bitmap_copy(other.bitmap_.get())) {}

RoaringBitmap::RoaringBitmap(RoaringBitmap&& other) noexcept
    : bitmap_(std::move(other.bitmap_)) {}

RoaringBitmap::~RoaringBitmap() = default;

RoaringBitmap& RoaringBitmap::operator=(const RoaringBitmap& other) {
    if (this != &other) {
        bitmap_.reset(roaring_bitmap_copy(other.bitmap_.get()));
    }
    return *this;
}

RoaringBitmap& RoaringBitmap::operator=(RoaringBitmap&& other) noexcept {
    if (this != &other) {
        bitmap_ = std::move(other.bitmap_);
    }
    return *this;
}

void RoaringBitmap::add(uint32_t value) {
    roaring_bitmap_add(bitmap_.get(), value);
}

void RoaringBitmap::remove(uint32_t value) {
    roaring_bitmap_remove(bitmap_.get(), value);
}

bool RoaringBitmap::contains(uint32_t value) const {
    return roaring_bitmap_contains(bitmap_.get(), value);
}

uint64_t RoaringBitmap::cardinality() const {
    return roaring_bitmap_get_cardinality(bitmap_.get());
}

bool RoaringBitmap::isEmpty() const {
    return roaring_bitmap_is_empty(bitmap_.get());
}

RoaringBitmap RoaringBitmap::operator&(const RoaringBitmap& other) const {
    RoaringBitmap result;
    result.bitmap_.reset(roaring_bitmap_and(bitmap_.get(), other.bitmap_.get()));
    return result;
}

RoaringBitmap RoaringBitmap::operator|(const RoaringBitmap& other) const {
    RoaringBitmap result;
    result.bitmap_.reset(roaring_bitmap_or(bitmap_.get(), other.bitmap_.get()));
    return result;
}

RoaringBitmap RoaringBitmap::operator-(const RoaringBitmap& other) const {
    RoaringBitmap result;
    result.bitmap_.reset(roaring_bitmap_andnot(bitmap_.get(), other.bitmap_.get()));
    return result;
}

RoaringBitmap RoaringBitmap::operator^(const RoaringBitmap& other) const {
    RoaringBitmap result;
    result.bitmap_.reset(roaring_bitmap_xor(bitmap_.get(), other.bitmap_.get()));
    return result;
}

RoaringBitmap& RoaringBitmap::operator&=(const RoaringBitmap& other) {
    roaring_bitmap_and_inplace(bitmap_.get(), other.bitmap_.get());
    return *this;
}

RoaringBitmap& RoaringBitmap::operator|=(const RoaringBitmap& other) {
    roaring_bitmap_or_inplace(bitmap_.get(), other.bitmap_.get());
    return *this;
}

RoaringBitmap& RoaringBitmap::operator-=(const RoaringBitmap& other) {
    roaring_bitmap_andnot_inplace(bitmap_.get(), other.bitmap_.get());
    return *this;
}

RoaringBitmap& RoaringBitmap::operator^=(const RoaringBitmap& other) {
    roaring_bitmap_xor_inplace(bitmap_.get(), other.bitmap_.get());
    return *this;
}

RoaringBitmap::Iterator::Iterator(const RoaringBitmap& bitmap, bool end) 
    : bitmap_(bitmap), current_index_(0), cardinality_(bitmap.cardinality()) {
    
    if (!end && cardinality_ > 0) {
        roaring_uint32_iterator_t iter;
        roaring_init_iterator(bitmap_.bitmap_.get(), &iter);
        current_value_ = iter.current_value;
    } else {
        current_index_ = cardinality_;
    }
}

uint32_t RoaringBitmap::Iterator::operator*() const {
    return current_value_;
}

RoaringBitmap::Iterator& RoaringBitmap::Iterator::operator++() {
    if (current_index_ < cardinality_ - 1) {
        current_index_++;
        
        roaring_uint32_iterator_t iter;
        roaring_init_iterator(bitmap_.bitmap_.get(), &iter);
        for (uint64_t i = 0; i < current_index_; i++) {
            roaring_advance_uint32_iterator(&iter);
        }
        current_value_ = iter.current_value;
    } else {
        current_index_ = cardinality_;
    }
    return *this;
}

bool RoaringBitmap::Iterator::operator!=(const Iterator& other) const {
    return current_index_ != other.current_index_;
}

RoaringBitmap::Iterator RoaringBitmap::begin() const {
    return Iterator(*this);
}

RoaringBitmap::Iterator RoaringBitmap::end() const {
    return Iterator(*this, true);
}

roaring_bitmap_t* RoaringBitmap::getRawBitmap() const {
    return bitmap_.get();
}