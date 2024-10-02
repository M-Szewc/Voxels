if [%1]==[] goto no_args
if [%2]==[] goto one_arg

: no_args
C:\VulkanSDK\1.3.290.0\Bin\glslc.exe shader.vert -o compiled\vertex.spv
C:\VulkanSDK\1.3.290.0\Bin\glslc.exe shader.frag -o compiled\fragment.spv
exit /b 1

:one_arg
C:\VulkanSDK\1.3.290.0\Bin\glslc.exe %1\shader.vert -o %1\compiled\vertex.spv
C:\VulkanSDK\1.3.290.0\Bin\glslc.exe %1\shader.frag -o %1\compiled\fragment.spv