# glslc is part of https://github.com/google/shaderc (and is included in the Vulkan SDK).
# The output of this script is the c++ code that can be found in BackendRendererVulkan.cpp

glslc -O shader.vert -o shader.vert.spv
glslc -O shader.frag -o shader.frag.spv

python3 -c "import struct; b=open('shader.vert.spv', 'rb').read(); w=struct.unpack('<%dI' % (len(b) // 4), b); print('    static const std::uint32_t vertexShaderSPV[] =\n        {' + ',\n         '.join(', '.join('0x{:08x}'.format(x) for x in w[i:i+10]) for i in range(0, len(w), 10)) + '};')"

echo ''

python3 -c "import struct; b=open('shader.frag.spv', 'rb').read(); w=struct.unpack('<%dI' % (len(b) // 4), b); print('    static const std::uint32_t fragmentShaderSPV[] =\n        {' + ',\n         '.join(', '.join('0x{:08x}'.format(x) for x in w[i:i+10]) for i in range(0, len(w), 10)) + '};')"
