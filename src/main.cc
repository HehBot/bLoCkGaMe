#include <buffer.h>
#include <camera.h>
#include <context.h>
#include <event.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <renderer.h>
#include <shader.h>
#include <texture.h>
#include <vertexarray.h>
#include <window.h>

using namespace graphics;

static float constexpr cube[6][6][5] = {
    { // +x
      {
          // coord
          0.5f,
          0.5f,
          0.5f,
          // tex
          0.0f,
          1.0f,
      },
      {
          0.5f,
          -0.5f,
          -0.5f,
          1.0f,
          0.0f,
      },
      {
          0.5f,
          0.5f,
          -0.5f,
          1.0f,
          1.0f,
      },
      {
          0.5f,
          -0.5f,
          -0.5f,
          1.0f,
          0.0f,
      },
      {
          0.5f,
          0.5f,
          0.5f,
          0.0f,
          1.0f,
      },
      {
          0.5f,
          -0.5f,
          0.5f,
          0.0f,
          0.0f,
      } },
    { // -x
      {
          -0.5f,
          0.5f,
          0.5f,
          1.0f,
          1.0f,
      },
      {
          -0.5f,
          0.5f,
          -0.5f,
          0.0f,
          1.0f,
      },
      {
          -0.5f,
          -0.5f,
          -0.5f,
          0.0f,
          0.0f,
      },
      {
          -0.5f,
          -0.5f,
          -0.5f,
          0.0f,
          0.0f,
      },
      {
          -0.5f,
          -0.5f,
          0.5f,
          1.0f,
          0.0f,
      },
      {
          -0.5f,
          0.5f,
          0.5f,
          1.0f,
          1.0f,
      } },
    { // +y
      {
          -0.5f,
          0.5f,
          -0.5f,
          0.0f,
          1.0f,
      },
      {
          0.5f,
          0.5f,
          0.5f,
          1.0f,
          0.0f,
      },
      {
          0.5f,
          0.5f,
          -0.5f,
          1.0f,
          1.0f,
      },
      {
          0.5f,
          0.5f,
          0.5f,
          1.0f,
          0.0f,
      },
      {
          -0.5f,
          0.5f,
          -0.5f,
          0.0f,
          1.0f,
      },
      {
          -0.5f,
          0.5f,
          0.5f,
          0.0f,
          0.0f,
      } },
    { // -y
      {
          -0.5f,
          -0.5f,
          -0.5f,
          1.0f,
          1.0f,
      },
      {
          0.5f,
          -0.5f,
          -0.5f,
          0.0f,
          1.0f,
      },
      {
          0.5f,
          -0.5f,
          0.5f,
          0.0f,
          0.0f,
      },
      {
          0.5f,
          -0.5f,
          0.5f,
          0.0f,
          0.0f,
      },
      {
          -0.5f,
          -0.5f,
          0.5f,
          1.0f,
          0.0f,
      },
      {
          -0.5f,
          -0.5f,
          -0.5f,
          1.0f,
          1.0f,
      } },
    { // +z
      {
          -0.5f,
          -0.5f,
          0.5f,
          0.0f,
          0.0f,
      },
      {
          0.5f,
          -0.5f,
          0.5f,
          1.0f,
          0.0f,
      },
      {
          0.5f,
          0.5f,
          0.5f,
          1.0f,
          1.0f,
      },
      {
          0.5f,
          0.5f,
          0.5f,
          1.0f,
          1.0f,
      },
      {
          -0.5f,
          0.5f,
          0.5f,
          0.0f,
          1.0f,
      },
      {
          -0.5f,
          -0.5f,
          0.5f,
          0.0f,
          0.0f,
      } },
    { // -z
      {
          -0.5f,
          -0.5f,
          -0.5f,
          1.0f,
          0.0f,
      },
      {
          0.5f,
          0.5f,
          -0.5f,
          0.0f,
          1.0f,
      },
      {
          0.5f,
          -0.5f,
          -0.5f,
          0.0f,
          0.0f,
      },
      {
          0.5f,
          0.5f,
          -0.5f,
          0.0f,
          1.0f,
      },
      {
          -0.5f,
          -0.5f,
          -0.5f,
          1.0f,
          0.0f,
      },
      {
          -0.5f,
          0.5f,
          -0.5f,
          1.0f,
          1.0f,
      } },
};

float constexpr colors[3][3] = {
    { 1.0, 0.0, 0.0 },
    { 0.0, 1.0, 0.0 },
    { 0.0, 0.0, 1.0 }
};

int main()
{
    std::shared_ptr<Context> context = Context::create();

    std::unique_ptr<Window> window = Window::create(context, Window::Prop { "graphics", 1000, 1000 });
    window->bind();
    std::unique_ptr<Renderer> renderer = Renderer::create(window.get(), { Renderer::Option::DepthTest });

    Camera camera(glm::vec3(0, 0, 20), glm::vec3(0.0, 0.0, -1.0));
    bool window_should_close = false;
    std::function<void(Event&)> callback = [&renderer, &window, &window_should_close, &camera](Event& event) {
        event.handled = true;
        switch (event.get_type()) {
        case EventType::WindowClose:
            window_should_close = true;
            break;
        case EventType::KeyPressed: {
            KeyPressedEvent& e = dynamic_cast<KeyPressedEvent&>(event);
            auto const k = e.get_key();
            if (k == Key::ESCAPE)
                window_should_close = true;
            else if (k == Key::UP || k == Key::W)
                camera.moveFront(1);
            else if (k == Key::DOWN || k == Key::S)
                camera.moveFront(-1);
            else if (k == Key::RIGHT || k == Key::D)
                camera.moveRight(1);
            else if (k == Key::LEFT || k == Key::A)
                camera.moveRight(-1);
            else
                std::cout << "Unhandled key " << (int)k << '\n';
            break;
        }
        case EventType::MouseMoved: {
            MouseMovedEvent& e = dynamic_cast<MouseMovedEvent&>(event);
            float const xpos = e.get_x(), ypos = e.get_y();
            double yaw, pitch;
            yaw = glm::radians(360.0 * xpos / window->get_width() - 270.0);
            pitch = glm::radians(90.0 - 180.0 * ypos / window->get_height());
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

    std::shared_ptr<Shader> shader = Shader::create({ "res/shaders/vert.glsl", "res/shaders/frag.glsl" });
    shader->bind();

    std::ifstream f("res/chunk");
    std::vector<float> vertices;
    size_t constexpr cz = 32;
    size_t vertex_count = 0;
    for (size_t x = 0; x < cz; ++x)
        for (size_t y = 0; y < cz; ++y)
            for (size_t z = 0; z < cz; ++z) {
                size_t c;
                f >> c;
                if (c != 0) {
                    vertex_count += 36;
                    for (size_t f = 0; f < 6; ++f) {
                        for (size_t v = 0; v < 6; ++v) {
                            vertices.push_back(x + cube[f][v][0]);
                            vertices.push_back(y + cube[f][v][1]);
                            vertices.push_back(z + cube[f][v][2]);
                            vertices.push_back(colors[c - 1][0]);
                            vertices.push_back(colors[c - 1][1]);
                            vertices.push_back(colors[c - 1][2]);
                        }
                    }
                }
            }

    std::shared_ptr<VertexBuffer> vbo = VertexBuffer::create(&vertices[0], sizeof(vertices[0]) * vertices.size());
    vbo->set_layout({ { ShaderDataType::Float3, "aPos" }, { ShaderDataType::Float3, "aColor" } });

    std::shared_ptr<VertexArray> vao = VertexArray::create();
    vao->bind();

    vao->add_vertex_buffer(vbo);

    renderer->set_clear_color({ 0.2f, 0.3f, 0.3f, 1.0f });

    while (!window_should_close) {
        renderer->clear();
        shader->set_mat4("projection", camera.projection(window->get_width() * 1.0f / window->get_height()));
        shader->set_mat4("view", camera.view());
        renderer->draw_triangles(vao, vertex_count);
        window->on_update();
    }

    return 0;
}
