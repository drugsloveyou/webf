/*
 * Copyright (C) 2019 Alibaba Inc. All rights reserved.
 * Author: Kraken Team.
 */

#include "window.h"
#include "logging.h"
#include <cassert>
#include "dart_callbacks.h"

namespace kraken {
namespace binding {

using namespace alibaba::jsa;

void JSWindow::invokeOnloadCallback(std::unique_ptr<JSContext> &context) {
  if (_onloadCallback.isUndefined()) {
    return;
  }

  auto funcObject = _onloadCallback.getObject(*context);

  if (funcObject.isFunction(*context)) {
    funcObject.asFunction(*context).call(*context);
  } else {
    KRAKEN_LOG(VERBOSE) << "__bind_load__ callback is not a function";
  }
}

Value JSWindow::get(JSContext &context,
                                  const PropNameID &name) {
  auto _name = name.utf8(context);
  if (_name == "devicePixelRatio") {
    if (getDartFunc()->devicePixelRatio == nullptr) {
      KRAKEN_LOG(ERROR) << "devicePixelRatio dart callback not register";
      return Value::undefined();
    }

    double devicePixelRatio = getDartFunc()->devicePixelRatio();
    return Value(devicePixelRatio);
  } else if (_name == "location") {
    return Value(context, Object::createFromHostObject(context, location_->shared_from_this()));
  }

  return Value::undefined();
}

void JSWindow::set(JSContext &context, const PropNameID &name, const Value &value) {
  auto _name = name.utf8(context);
  if (_name == "onload") {
    _onloadCallback = Value(context, value);
  }
}

void JSWindow::bind(std::unique_ptr<JSContext> &context) {
  assert(context != nullptr);

  Object&& window =
      Object::createFromHostObject(*context, sharedSelf());
  location_->bind(context, window);
  JSA_GLOBAL_SET_PROPERTY(*context, "__kraken_window__", window);
}

void JSWindow::unbind(std::unique_ptr<JSContext> &context) {
  Value &&window = JSA_GLOBAL_GET_PROPERTY(*context, "__kraken_window__");
  Object &&object = window.getObject(*context);
  location_->unbind(context, object);
  _onloadCallback = Value::undefined();
  JSA_GLOBAL_SET_PROPERTY(*context, "__kraken_window__", Value::undefined());
}

std::vector<PropNameID> JSWindow::getPropertyNames(JSContext &context) {
  std::vector<PropNameID> names;
  names.emplace_back(PropNameID::forUtf8(context, "devicePixelRatio"));
  names.emplace_back(PropNameID::forUtf8(context, "location"));
  return names;
}

} // namespace binding
} // namespace kraken
