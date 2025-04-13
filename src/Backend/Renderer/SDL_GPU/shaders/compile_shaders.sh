# shadercross is the CLI tool of https://github.com/libsdl-org/SDL_shadercross

shadercross shader.vert.hlsl -o shader.vert.spv
shadercross shader.vert.hlsl -o shader.vert.msl
shadercross shader.vert.hlsl -o shader.vert.dxil
shadercross shader.frag.hlsl -o shader.frag.spv
shadercross shader.frag.hlsl -o shader.frag.msl
shadercross shader.frag.hlsl -o shader.frag.dxil

echo '    static const std::uint8_t vertexShaderMSL[] = R"('
cat shader.vert.msl | sed 's/^/        /'
echo '    )";'
echo ''

echo '    static const std::uint8_t fragmentShaderMSL[] = R"('
cat shader.frag.msl | sed 's/^/        /'
echo '    )";'
echo ''

python -c "b=open('shader.vert.spv', 'rb').read(); print('    static const std::uint8_t vertexShaderSPV[] = \n        \"' + '\"\n        \"'.join(''.join(r'\x{:02x}'.format(c) for c in b[i:i+40]) for i in range(0, len(b), 40)) + '\";')"

echo ''

python -c "b=open('shader.frag.spv', 'rb').read(); print('    static const std::uint8_t fragmentShaderSPV[] = \n        \"' + '\"\n        \"'.join(''.join(r'\x{:02x}'.format(c) for c in b[i:i+40]) for i in range(0, len(b), 40)) + '\";')"

echo ''

python -c "b=open('shader.vert.dxil', 'rb').read(); print('    static const std::uint8_t vertexShaderDXIL[] = \n        \"' + '\"\n        \"'.join(''.join(r'\x{:02x}'.format(c) for c in b[i:i+40]) for i in range(0, len(b), 40)) + '\";')"

echo ''

python -c "b=open('shader.frag.dxil', 'rb').read(); print('    static const std::uint8_t fragmentShaderDXIL[] = \n        \"' + '\"\n        \"'.join(''.join(r'\x{:02x}'.format(c) for c in b[i:i+40]) for i in range(0, len(b), 40)) + '\";')"
