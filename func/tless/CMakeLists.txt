# Different functions for the TLess experiments
set(FAASM_USER tless)

# Func 1: Pre-processing function in C++, chains to Func 2 and Func 3
faasm_func(pre pre.cpp)

# Func 2: Image processing with ImageMagick (cross-compiled with `inv imagemagick`)

# Func 3: Inference function with TensorFlow Lite
add_subdirectory(inference)

# Func 4: post-process inference results
faasm_func(post_tf post_tf.cpp)

# --------------------
# Micro-Benchmark Functions
# --------------------

# Memory-intensive video transcoding using FFmpeg
add_subdirectory(transcode)
