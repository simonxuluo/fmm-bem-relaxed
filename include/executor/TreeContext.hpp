#pragma once

#include "TransformIterator.hpp"

template <typename Tree, typename Indexable>
struct body_map {
  typedef body_map<Tree,Indexable> self_type;
  typedef typename Indexable::reference reference;
  typedef typename Tree::box_type box_type;
  typedef typename Tree::body_type body_type;
  typedef typename Tree::body_iterator body_iterator;

  // Default Constructor
  body_map() {
  }
  // Constructor
  body_map(const Indexable& body_value)
    : body_value_(body_value) {
  }
  // RValue constructor for efficiency
  body_map(Indexable&& body_value)
    : body_value_(std::move(body_value)) {
  }

  const reference operator()(const body_type& b) const {
    return body_value_[b.number()];  // TODO TEMP: number to workaround permute
  }
  reference operator()(const body_type& b) {
    return body_value_[b.number()];  // TODO TEMP: number to workaround permute
  }

  // self_type& to prevent copying, Indexable may be a heavy container
  typedef transform_iterator<body_iterator, const self_type&> body_value_const_iterator;
  body_value_const_iterator begin(const box_type& box) const {
    return make_transform_iterator<const self_type&>(box.body_begin(), *this);
  }
  body_value_const_iterator end(const box_type& box) const {
    return make_transform_iterator<const self_type&>(box.body_end(), *this);
  }

  typedef transform_iterator<body_iterator, self_type&> body_value_iterator;
  body_value_iterator begin(const box_type& box) {
    return make_transform_iterator<self_type&>(box.body_begin(), *this);
  }
  body_value_iterator end(const box_type& box) {
    return make_transform_iterator<self_type&>(box.body_end(), *this);
  }

  Indexable& data() {
    return body_value_;
  }
  self_type& operator=(const Indexable& v) {
    body_value_ = v;
    return *this;
  }

private:
  Indexable body_value_;
};


template <typename Tree, typename Indexable>
struct box_map {
  typedef box_map<Tree,Indexable> self_type;
  typedef typename Indexable::value_type value_type;
  typedef typename Tree::box_type box_type;

  // Default Constructor
  box_map() {
  }
  // Constructor
  box_map(const Indexable& box_value)
    : box_value_(box_value) {
  }
  // RValue constructor for efficiency
  box_map(Indexable&& box_value)
    : box_value_(std::move(box_value)) {
  }

  const value_type& operator()(const box_type& box) const {
    return box_value_[box.index()];
  }
  value_type& operator()(const box_type& box) {
    return box_value_[box.index()];
  }

  Indexable& data() {
    return box_value_;
  }

private:
  Indexable box_value_;
};
