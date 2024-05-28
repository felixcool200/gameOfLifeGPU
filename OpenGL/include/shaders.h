#ifndef SHADERS_H
#define SHADERS_H

#include "consts.h"

//Shaders
const char* computeShaderSource = R"(
#version 430 core
layout (local_size_x = )" STR(DIV_FACTOR) R"(, local_size_y = )" STR(DIV_FACTOR) R"() in;
layout (rgba32f, binding = 0) uniform writeonly image2D img_output;
layout (binding = 1) uniform sampler2D img_input;

ivec2 directions[8] = ivec2[8](
    ivec2(-1, -1), ivec2(0, -1), ivec2(1, -1),
    ivec2(-1,  0),             ivec2(1,  0),
    ivec2(-1,  1), ivec2(0,  1), ivec2(1,  1)
);

void main() {
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    //if (pixel_coords.x < 0 && pixel_coords.x >= )" STR(WIDTH) R"( &&
    //    pixel_coords.y < 0 && pixel_coords.y >= )" STR(HEIGHT) R"() {
    //        return;
    //    }
    vec4 current_state = texelFetch(img_input, pixel_coords, 0);
    int alive_neighbors = 0;

    // Count alive neighbors
    for (int i = 0; i < 8; ++i) {
        ivec2 neighbor_coords = pixel_coords + directions[i];
        //if (neighbor_coords.x >= 0 && neighbor_coords.x < )" STR(WIDTH) R"( &&
        //    neighbor_coords.y >= 0 && neighbor_coords.y < )" STR(HEIGHT) R"() {
            vec4 neighbor_state = texelFetch(img_input, neighbor_coords, 0);
            alive_neighbors += int(neighbor_state.r);
        //}
    }

    // Apply Game of Life rules
    vec4 next_state = vec4(0.0);
    if (current_state.r == 1.0) {
        // Current cell is alive
        if (alive_neighbors == 2 || alive_neighbors == 3) {
            next_state.rgb = vec3(1, 1, 1); // Stays alive
        } else {
            next_state.rgb = vec3(0, 0, 0); // Dies
        }
    } else {
        // Current cell is dead
        if (alive_neighbors == 3) {
            next_state.rgb = vec3(1, 1, 1); // Becomes alive
        } else {
            next_state.rgb = vec3(0, 0, 0); // Stays dead
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


const char* vertexShaderINTSource = R"(
    #version 430 core
    layout (location = 0) in ivec2 aPos;
    layout (location = 1) in ivec2 aTexCoord;
    
    out flat ivec2 TexCoord;
    
    void main() {
        gl_Position = vec4(aPos, 0.0, 1.0);
        TexCoord = aTexCoord;
    }
    )";

const char* fragmentAndComputeShaderSource = R"glsl(
    #version 430 core
    out vec4 FragColor;
    in flat ivec2 TexCoord;
    layout (rgba32f, binding = 0) uniform writeonly image2D img_output;
    layout (binding = 1) uniform sampler2D img_input;

    ivec2 directions[8] = ivec2[8](
        ivec2(-1, -1), ivec2(0, -1), ivec2(1, -1),
        ivec2(-1,  0),             ivec2(1,  0),
        ivec2(-1,  1), ivec2(0,  1), ivec2(1,  1)
    );

    void main() {
        ivec2 pixel_coords = TexCoord;
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
                next_state.rgb = vec3(1, 1, 1); // Stays alive
            } else {
                next_state.rgb = vec3(0, 0, 0); // Dies
            }
        } else {
            // Current cell is dead
            if (alive_neighbors == 3) {
                next_state.rgb = vec3(1, 1, 1); // Becomes alive
            } else {
                next_state.rgb = vec3(0, 0, 0); // Stays dead
            }
        }
        // Store to new texture for next calulation
        imageStore(img_output, pixel_coords, next_state);
        
        // Output to screen
        FragColor = next_state;
    })glsl";

#endif