// internal
#include "render.hpp"
#include "render_components.hpp"
#include "tiny_ecs.hpp"
#include "blobule.hpp"
#include "text.hpp"
#include <iostream>
#include <egg.hpp>
#include <helptool.hpp>

float ANIMATION_FREQUENCY = 500.f; // Milliseconds between sprite frame updates
float time_elapsed = 0.f;
float frame_number = 1.f; // Start all sprite sheets at first frame

void RenderSystem::drawTexturedMesh(ECS::Entity entity, const mat3& projection)
{
	auto& motion = ECS::registry<Motion>.get(entity);
	auto& texmesh = *ECS::registry<ShadedMeshRef>.get(entity).reference_to_cache;
	// Transformation code, see Rendering and Transformation in the template specification for more info
	// Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
	Transform transform;
	transform.translate(motion.position);
	transform.rotate(motion.angle);
	transform.scale(motion.scale);

	// Setting shaders
	glUseProgram(texmesh.effect.program);
	glBindVertexArray(texmesh.mesh.vao);
	gl_has_errors();

	// Enabling alpha channel for textures
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	gl_has_errors();

	GLint transform_uloc = glGetUniformLocation(texmesh.effect.program, "transform");
	GLint projection_uloc = glGetUniformLocation(texmesh.effect.program, "projection");
	gl_has_errors();

	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, texmesh.mesh.vbo);

	// If the texmesh has more than one row and column (ie. is a sprite sheet)
	if (texmesh.num_rows > 1.f && texmesh.num_columns > 1.f)
	{
		TexturedVertex vertices[4];
		vertices[0].position = { -1.f / 2, +1.f / 2, 0.f };
		vertices[1].position = { +1.f / 2, +1.f / 2, 0.f };
		vertices[2].position = { +1.f / 2, -1.f / 2, 0.f };
		vertices[3].position = { -1.f / 2, -1.f / 2, 0.f };

		// Blobule specific animations
		if (ECS::registry<Blobule>.has(entity))
		{
			// If the blobule is moving we're using the first row in the sprite sheet
			if (abs(motion.velocity.x) > 0.f || abs(motion.velocity.y) > 0.f)
			{
				vertices[0].texcoord = { (frame_number - 1.f) / texmesh.num_columns, 1.f };
				vertices[1].texcoord = { frame_number / texmesh.num_columns, 1.f };
				vertices[2].texcoord = { frame_number / texmesh.num_columns, 1.f / texmesh.num_rows };
				vertices[3].texcoord = { (frame_number - 1.f) / texmesh.num_columns, 1.f / texmesh.num_rows };
			}
			// If the blobule is not moving we're using the second row in the sprite sheet
			else
			{
				vertices[0].texcoord = { (frame_number - 1.f) / texmesh.num_columns, 1.f / texmesh.num_rows };
				vertices[1].texcoord = { frame_number / texmesh.num_columns, 1.f / texmesh.num_rows };
				vertices[2].texcoord = { frame_number / texmesh.num_columns, 0.f };
				vertices[3].texcoord = { (frame_number - 1.f) / texmesh.num_columns, 0.f };
			}
		}
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, texmesh.mesh.ibo);
	gl_has_errors();

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(texmesh.effect.program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(texmesh.effect.program, "in_texcoord");
	GLint in_color_loc = glGetAttribLocation(texmesh.effect.program, "in_color");
	if (in_texcoord_loc >= 0)
	{
		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), reinterpret_cast<void*>(0));
		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), reinterpret_cast<void*>(sizeof(vec3))); // note the stride to skip the preceeding vertex position
		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texmesh.texture.texture_id);
	}
	else if (in_color_loc >= 0)
	{
		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), reinterpret_cast<void*>(0));
		glEnableVertexAttribArray(in_color_loc);
		glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), reinterpret_cast<void*>(sizeof(vec3)));
	}
	else
	{
		throw std::runtime_error("This type of entity is not yet supported");
	}
	gl_has_errors();

	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(texmesh.effect.program, "fcolor");
	glUniform3fv(color_uloc, 1, (float*)&texmesh.texture.color);
	gl_has_errors();

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();
	GLsizei num_indices = size / sizeof(uint16_t);
	//GLsizei num_triangles = num_indices / 3;

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform.mat);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	gl_has_errors();

	// Drawing of num_indices/3 triangles specified in the index buffer
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	glBindVertexArray(0);
}

// Draw the intermediate texture to the screen, with some distortion to simulate water
void RenderSystem::drawToScreen()
{
	// Setting shaders
	glUseProgram(screen_sprite.effect.program);
	glBindVertexArray(screen_sprite.mesh.vao);
	gl_has_errors();

	// Clearing backbuffer
	int w, h;
	glfwGetFramebufferSize(&window, &w, &h);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, w, h);
	glDepthRange(0, 10);
	glClearColor(1.f, 0, 0, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_has_errors();

	// Disable alpha channel for mapping the screen texture onto the real screen
	glDisable(GL_BLEND); // we have a single texture without transparency. Areas with alpha <1 cab arise around the texture transparency boundary, enabling blending would make them visible.
	glDisable(GL_DEPTH_TEST);

	glBindBuffer(GL_ARRAY_BUFFER, screen_sprite.mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, screen_sprite.mesh.ibo); // Note, GL_ELEMENT_ARRAY_BUFFER associates indices to the bound GL_ARRAY_BUFFER
	gl_has_errors();

	// Draw the screen texture on the quad geometry
	gl_has_errors();

	// Set clock
	GLuint time_uloc       = glGetUniformLocation(screen_sprite.effect.program, "time");
	GLuint dead_timer_uloc = glGetUniformLocation(screen_sprite.effect.program, "darken_screen_factor");
	glUniform1f(time_uloc, static_cast<float>(glfwGetTime() * 10.0f));
	auto& screen = ECS::registry<ScreenState>.get(screen_state_entity);
	glUniform1f(dead_timer_uloc, screen.darken_screen_factor);
	gl_has_errors();

	// Set the vertex position and vertex texture coordinates (both stored in the same VBO)
	GLint in_position_loc = glGetAttribLocation(screen_sprite.effect.program, "in_position");
	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
	GLint in_texcoord_loc = glGetAttribLocation(screen_sprite.effect.program, "in_texcoord");
	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3)); // note the stride to skip the preceeding vertex position
	gl_has_errors();

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screen_sprite.texture.texture_id);

	// Draw
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr); // two triangles = 6 vertices; nullptr indicates that there is no offset from the bound index buffer
	glBindVertexArray(0);
	gl_has_errors();
}

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void RenderSystem::draw(float elapsed_ms, vec2 window_size_in_game_units)
{
	time_elapsed += elapsed_ms;
	if (time_elapsed > ANIMATION_FREQUENCY) {
		time_elapsed = 0.f;
		frame_number++;
		if (frame_number > 3) // Hardcoded to 3 frames per row, will have a better solution soon!
			frame_number = 1;
	}

	// Getting size of window
	ivec2 frame_buffer_size; // in pixels
	glfwGetFramebufferSize(&window, &frame_buffer_size.x, &frame_buffer_size.y);

	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	gl_has_errors();

	// Clearing backbuffer
	glViewport(0, 0, frame_buffer_size.x, frame_buffer_size.y);
	glDepthRange(0.00001, 10);
    // Background colour is {R, G, B, A}
	glClearColor(1, 1, 1, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_has_errors();

	// Fake projection matrix, scales with respect to window coordinates
	float left = 0.f;
	float top = 0.f;
	float right = window_size_in_game_units.x;
	float bottom = window_size_in_game_units.y;

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	mat3 projection_2D{ { sx, 0.f, 0.f },{ 0.f, sy, 0.f },{ tx, ty, 1.f } };

	std::vector<ECS::Entity> overlay;
	std::vector<ECS::Entity> firstEntities;
	std::vector<ECS::Entity> secondEntities;
	std::vector<ECS::Entity> thirdEntities;

	for (ECS::Entity entity : ECS::registry<ShadedMeshRef>.entities)
	{
		if (ECS::registry<HelpTool>.has(entity)) {
			overlay.push_back(entity);
		}
		if (ECS::registry<Blobule>.has(entity) || ECS::registry<Egg>.has(entity)) {
			firstEntities.push_back(entity);
		}
		else if (ECS::registry<BlueSplat>.has(entity) || ECS::registry<RedSplat>.has(entity) || ECS::registry<YellowSplat>.has(entity) || ECS::registry<RedSplat>.has(entity)) {
			secondEntities.push_back(entity);
		}
		else {
			thirdEntities.push_back(entity);
		}
	}
	// Renders tiles and other thirdlevel entities
	for (ECS::Entity entity : thirdEntities)
	{
		if (!ECS::registry<Motion>.has(entity))
			continue;
		// Note, its not very efficient to access elements indirectly via the entity albeit iterating through all Sprites in sequence
		drawTexturedMesh(entity, projection_2D);
		gl_has_errors();
	}

	// Renders splats and other second level entities
	for (ECS::Entity entity : secondEntities)
	{
		if (!ECS::registry<Motion>.has(entity))
			continue;
		// Note, its not very efficient to access elements indirectly via the entity albeit iterating through all Sprites in sequence
		drawTexturedMesh(entity, projection_2D);
		gl_has_errors();
	}

	// renders blobs and eggs and other first level entities
	for (ECS::Entity entity : firstEntities)
	{
		if (!ECS::registry<Motion>.has(entity))
			continue;
		// Note, its not very efficient to access elements indirectly via the entity albeit iterating through all Sprites in sequence
		drawTexturedMesh(entity, projection_2D);
		gl_has_errors();
	}

	// renders helptool and other overlay level entities
	for (ECS::Entity entity : overlay)
	{
		if (!ECS::registry<Motion>.has(entity))
			continue;
		// Note, its not very efficient to access elements indirectly via the entity albeit iterating through all Sprites in sequence
		drawTexturedMesh(entity, projection_2D);
		gl_has_errors();
	}

	// Draw text components to the screen
	// NOTE: for simplicity, text components are drawn in a second pass,
	// on top of all texture mesh components. This should be reasonable
	// for nearly all use cases. If you need text to appear behind meshes,
	// consider using a depth buffer during rendering and adding a
	// Z-component or depth index to all rendererable components.
	for (const Text& text : ECS::registry<Text>.components) {
		drawText(text, window_size_in_game_units);
	}

	// Truely render to the screen
	drawToScreen();

	// flicker-free display with a double buffer
	glfwSwapBuffers(&window);
}

void gl_has_errors()
{
	GLenum error = glGetError();

	if (error == GL_NO_ERROR)
		return;

	const char* error_str = "";
	while (error != GL_NO_ERROR)
	{
		switch (error)
		{
		case GL_INVALID_OPERATION:
			error_str = "INVALID_OPERATION";
			break;
		case GL_INVALID_ENUM:
			error_str = "INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			error_str = "INVALID_VALUE";
			break;
		case GL_OUT_OF_MEMORY:
			error_str = "OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			error_str = "INVALID_FRAMEBUFFER_OPERATION";
			break;
		}

		std::cerr << "OpenGL:" << error_str << std::endl;
		error = glGetError();
	}
	throw std::runtime_error("last OpenGL error:" + std::string(error_str));
}
