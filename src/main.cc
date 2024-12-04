#include "camera.h"
#include "world.h"

#include <buffer.h>
#include <context.h>
#include <cstdlib>
#include <event.h>
#include <fstream>
#include <ios>
#include <iostream>
#include <memory>
#include <renderer.h>
#include <shader.h>
#include <texture.h>
#include <vector>
#include <vertexarray.h>
#include <window.h>

using namespace graphics;

#define RESDIR "res/"

int main()
{
    std::fstream f(RESDIR "/world", std::ios_base::binary | std::ios_base::in | std::ios_base::out);
    World* world = new World(f);

    std::shared_ptr<Context> context = Context::create();

    std::unique_ptr<Window> window = Window::create(context, Window::Prop { "blockGame", 1000, 1000 });
    window->bind();
    std::unique_ptr<Renderer> renderer = Renderer::create(window.get(), { Renderer::Option::DepthTest });

    Camera camera(glm::vec3(0, 0, 60), glm::vec3(0.0, 0.0, -1.0));
    bool window_should_close = false;
    struct {
        bool up = false, down = false, left = false, right = false;
    } button_held;
    std::function<void(Event&)> callback = [&renderer, &window, &window_should_close, &camera, &button_held](Event& event) {
        event.handled = true;
        switch (event.get_type()) {
        case EventType::WindowClose:
            window_should_close = true;
            break;
        case EventType::KeyPressed: {
            KeyPressedEvent& e = dynamic_cast<KeyPressedEvent&>(event);
            auto const k = e.get_key();
            switch (k) {
            case Key::ESCAPE:
                window_should_close = true;
                break;
            case Key::UP:
            case Key::W:
                button_held.up = true;
                break;
            case Key::DOWN:
            case Key::S:
                button_held.down = true;
                break;
            case Key::LEFT:
            case Key::A:
                button_held.left = true;
                break;
            case Key::RIGHT:
            case Key::D:
                button_held.right = true;
                break;
            case Key::L:
                renderer->enable_line_mode();
                break;
            case Key::F:
                renderer->enable_fill_mode();
                break;
            case Key::P:
                renderer->enable_point_mode();
                break;
            default:
                std::cout << "Unhandled key " << (int)k << '\n';
            }
            break;
        }
        case EventType::KeyReleased: {
            KeyReleasedEvent& e = dynamic_cast<KeyReleasedEvent&>(event);
            auto const k = e.get_key();
            if (k == Key::UP || k == Key::W)
                button_held.up = false;
            else if (k == Key::DOWN || k == Key::S)
                button_held.down = false;
            else if (k == Key::LEFT || k == Key::A)
                button_held.left = false;
            else if (k == Key::RIGHT || k == Key::D)
                button_held.right = false;
            break;
        }
        case EventType::MouseMoved: {
            MouseMovedEvent& e = dynamic_cast<MouseMovedEvent&>(event);
            float const xpos = e.get_x(), ypos = e.get_y();
            double yaw, pitch;
            yaw = glm::radians(360.0 * xpos / window->width - 270.0);
            pitch = glm::radians(90.0 - 180.0 * ypos / window->height);
            camera.updateFront(glm::vec3(cos(yaw) * cos(pitch), sin(pitch), sin(yaw) * cos(pitch)));
            break;
        }
        case EventType::WindowResize: {
            WindowResizeEvent& e = dynamic_cast<WindowResizeEvent&>(event);
            renderer->set_viewport(0, 0, e.get_width(), e.get_height());
            break;
        }
        default:
            event.handled = false;
        }
    };
    window->set_event_callback(callback);
    window->set_vsync(false);

    std::shared_ptr<Shader> shader = Shader::load({ RESDIR "/shaders/basic.vert", RESDIR "/shaders/basic.frag" });
    shader->bind();

    std::shared_ptr<Texture> tex_atlas = Texture2D::create(RESDIR "/textureAtlas(test).png");
    tex_atlas->bind_to_slot(0);
    shader->set_int("tex_atlas", 0);

    std::shared_ptr<VertexBuffer> vbo = VertexBuffer::create(MAX_CHUNKS * (size_t)32 * 32 * 32);
    vbo->set_layout({ { ShaderDataType::Float3, "aPos" }, { ShaderDataType::Float2, "aColor" } });

    std::shared_ptr<VertexArray> vao = VertexArray::create();
    vao->bind();

    vao->add_vertex_buffer(vbo);

    renderer->set_clear_color({ 0.2f, 0.3f, 0.3f, 1.0f });

    size_t vertex_count = 0;

    double start = window->get_time();
    size_t nr_frames = 0;
    window->init_delta_timer();
    while (!window_should_close) {
        renderer->clear();
        shader->set_mat4("projection", camera.projection(window->width * 1.0f / window->height));
        shader->set_mat4("view", camera.view());

        if (world->mesh_has_changed(camera.position().x, camera.position().y, camera.position().z)) {
            std::vector<float> v = world->mesh(camera.position().x, camera.position().y, camera.position().z);
            if (vbo->size() < v.size() * sizeof(v[0]))
                vbo->resize_and_clear(v.size() * sizeof(v[0]) * 2);
            vbo->set_data(&v[0], 0, v.size() * sizeof(v[0]));
            vertex_count = v.size() / 5;
        }
        renderer->draw_triangles(vao, vertex_count);

        window->on_update();

        double delta = window->get_delta_time();
        if (button_held.up && !button_held.down)
            camera.moveFront(delta);
        else if (button_held.down && !button_held.up)
            camera.moveFront(-delta);
        if (button_held.right && !button_held.left)
            camera.moveRight(delta);
        else if (button_held.left && !button_held.right)
            camera.moveRight(-delta);

        nr_frames++;
    }
    std::cout << "Frame rate: " << nr_frames / (window->get_time() - start) << "fps\n";
    std::cout << "#Triangles: " << vertex_count / 3 << '\n';

    delete world;

    return 0;
}
