
//Shaders
const char* computeShaderSource = R"(
#version 430 core
layout (local_size_x = 16, local_size_y = 16) in;
layout (rgba32f, binding = 0) uniform image2D img_output;
layout (binding = 1) uniform sampler2D img_input;

ivec2 directions[8] = ivec2[8](
    ivec2(-1, -1), ivec2(0, -1), ivec2(1, -1),
    ivec2(-1,  0),             ivec2(1,  0),
    ivec2(-1,  1), ivec2(0,  1), ivec2(1,  1)
);

void main() {
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    vec4 current_state = texelFetch(img_input, pixel_coords, 0);
    int alive_neighbors = 0;

    // Count alive neighbors
    for (int i = 0; i < 8; ++i) {
        ivec2 neighbor_coords = pixel_coords + directions[i];
        vec4 neighbor_state = texelFetch(img_input, neighbor_coords, 0);
        alive_neighbors += int(neighbor_state.r);
    }

    // Apply Game of Life rules
    vec4 next_state = vec4(0.0);
    if (current_state.r == 1.0) {
        // Current cell is alive
        if (alive_neighbors == 2 || alive_neighbors == 3) {
            next_state.rgb = vec3(1,1,1); // Stays alive
        }
        else {
            next_state.rgb = vec3(0,0,0); // Dies
        }
    } else {
        // Current cell is dead
        if (alive_neighbors == 3) {
            next_state.rgb = vec3(1,1,1); // Becomes alive
        }
        else {
            next_state.rgb = vec3(0,0,0); // Stays dead
        }
    }

    // Write the next state to the output image
    imageStore(img_output, pixel_coords, next_state);
}
)";

const char* vertexShaderSource = R"(
    #version 430 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTexCoord;
    
    out vec2 TexCoord;
    
    void main() {
        gl_Position = vec4(aPos, 0.0, 1.0);
        TexCoord = aTexCoord;
    }
    )";
    
const char* fragmentShaderSource =
    "#version 430 core\n"
    "out vec4 FragColor;\n"
    "in vec2 TexCoord;\n"
    "uniform sampler2D texture1;\n"
    "void main() {\n"
    "    FragColor = texture(texture1, TexCoord);\n"
    "}\n";

/*
const char* computeShaderSource = 
    "#version 430 core\n"
    "layout (local_size_x = 16, local_size_y = 16) in;\n"
    "layout (rgba32f, binding = 0) uniform image2D img_output;\n"
    "layout (binding = 1) uniform sampler2D img_input;\n"

    "void main() {\n"
    "    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);\n"
    "    vec4 current_state = texelFetch(img_input, pixel_coords, 0);\n"
    "    vec4 color = vec4(current_state.rgb, 1.0);\n"
    "    imageStore(img_output, pixel_coords, color);\n"
    "}\n";
*/
