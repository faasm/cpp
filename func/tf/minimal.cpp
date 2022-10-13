#include <faasm/faasm.h>
#include <faasm/input.h>

#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"
#include "tensorflow/lite/optional_debug_tools.h"

#define INPUT_FILE_PATH "faasm://tflite/sample_model.tflite"
#define TFLITE_MINIMAL_CHECK(x)                                                \
    if (!(x)) {                                                                \
        fprintf(stderr, "Error at %s:%d\n", __FILE__, __LINE__);               \
        exit(1);                                                               \
    }

/**
 * This is an example that is minimal to read a model
 * from disk to check if the lib was compiled successfully.
 *
 * Example inspired from:
 * https://github.com/tensorflow/tensorflow/blob/master/tensorflow/lite/examples/minimal/minimal.cc
 */

int main()
{
    // Load model from file
    std::unique_ptr<tflite::FlatBufferModel> model =
      tflite::FlatBufferModel::BuildFromFile(INPUT_FILE_PATH);
    TFLITE_MINIMAL_CHECK(model != nullptr);

    // Build the interpreter with the InterpreterBuilder.
    tflite::ops::builtin::BuiltinOpResolver resolver;
    tflite::InterpreterBuilder builder(*model, resolver);
    std::unique_ptr<tflite::Interpreter> interpreter;
    builder(&interpreter);
    TFLITE_MINIMAL_CHECK(interpreter != nullptr);

    // Allocate tensor buffers.
    TFLITE_MINIMAL_CHECK(interpreter->AllocateTensors() == kTfLiteOk);
    printf("=== Pre-invoke Interpreter State ===\n");
    tflite::PrintInterpreterState(interpreter.get());

    return 0;
}
