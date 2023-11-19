/*
 * Copyright (C) 2022-present The WebF authors. All rights reserved.
 */

#include "double_ui_command.h"
#include "core/executing_context.h"
#include "foundation/logging.h"
#include "ui_command_buffer.h"

namespace webf {


DoubleUICommand::DoubleUICommand(ExecutingContext* context)
    : frontBuffer(std::make_unique<UICommandBuffer>(context)), isSwapping(false) {
  auto* dart_isolate_context = context->dartIsolateContext();

  if (dart_isolate_context != nullptr && dart_isolate_context->dispatcher()->isDedicatedThread()) {
    WEBF_LOG(DEBUG) << "[CPP] DoubleUICommand::DoubleUICommand, create backBuffer" << std::endl;
    backBuffer = std::make_unique<UICommandBuffer>(context);
  }
}

void DoubleUICommand::addCommand(UICommand type,
                                 std::unique_ptr<SharedNativeString>&& args_01,
                                 void* nativePtr,
                                 void* nativePtr2,
                                 bool request_ui_update) {
  if (backBuffer == nullptr) {
    frontBuffer->addCommand(type, std::move(args_01), nativePtr, nativePtr2, request_ui_update);
    return;
  }

  while (isSwapping) {
    // simply spin wait for the swapBuffers to finish.
  }
  backBuffer->addCommand(type, std::move(args_01), nativePtr, nativePtr2, request_ui_update);
}

// first called by dart to begin read commands.
UICommandItem* DoubleUICommand::data() {
  if (backBuffer == nullptr) {
    return frontBuffer->data();
  }

  swapBuffers();
  return frontBuffer->data();
}

// second called by dart to get the size of commands.
int64_t DoubleUICommand::size() {
  return frontBuffer->size();
}

// third called by dart to clear commands.
void DoubleUICommand::clear() {
  frontBuffer->clear();
}

// called by c++ to check if there are commands.
bool DoubleUICommand::empty() {
  if (backBuffer == nullptr) {
    return frontBuffer->empty();
  }

  return backBuffer->empty();
}

UICommandBuffer* DoubleUICommand::getFrontBuffer() {
  swapBuffers();

  return frontBuffer.get();
}

void DoubleUICommand::swapBuffers() {
  if (backBuffer == nullptr) {
    return;
  }

  isSwapping = true;
  std::swap(frontBuffer, backBuffer);
  isSwapping = false;
}

}  // namespace webf