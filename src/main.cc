#include <buffer.h>
#include <camera.h>
#include <context.h>
#include <cstdlib>
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

#define chunk_file_path "res/chunk"
#define chunk_size 32

static size_t constexpr cube[6][6][5] = {
    { // +x
      // coord   tex
      { 1, 1, 1, 0, 1 },
      { 1, 0, 0, 1, 0 },
      { 1, 1, 0, 1, 1 },
      { 1, 0, 0, 1, 0 },
      { 1, 1, 1, 0, 1 },
      { 1, 0, 1, 0, 0 } },
    { // -x
      { 0, 1, 1, 1, 1 },
      { 0, 1, 0, 0, 1 },
      { 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0 },
      { 0, 0, 1, 1, 0 },
      { 0, 1, 1, 1, 1 } },
    { // +y
      { 0, 1, 0, 0, 1 },
      { 1, 1, 1, 1, 0 },
      { 1, 1, 0, 1, 1 },
      { 1, 1, 1, 1, 0 },
      { 0, 1, 0, 0, 1 },
      { 0, 1, 1, 0, 0 } },
    { // -y
      { 0, 0, 0, 1, 1 },
      { 1, 0, 0, 0, 1 },
      { 1, 0, 1, 0, 0 },
      { 1, 0, 1, 0, 0 },
      { 0, 0, 1, 1, 0 },
      { 0, 0, 0, 1, 1 } },
    { // +z
      { 0, 0, 1, 0, 0 },
      { 1, 0, 1, 1, 0 },
      { 1, 1, 1, 1, 1 },
      { 1, 1, 1, 1, 1 },
      { 0, 1, 1, 0, 1 },
      { 0, 0, 1, 0, 0 } },
    { // -z
      { 0, 0, 0, 1, 0 },
      { 1, 1, 0, 0, 1 },
      { 1, 0, 0, 0, 0 },
      { 1, 1, 0, 0, 1 },
      { 0, 0, 0, 1, 0 },
      { 0, 1, 0, 1, 1 } },
};

float constexpr colors[3][3] = {
    { 1.0, 0.0, 0.0 },
    { 0.0, 1.0, 0.0 },
    { 0.0, 0.0, 1.0 }
};

void push3(std::vector<float>& v, float const f[3])
{
    v.push_back(f[0]);
    v.push_back(f[1]);
    v.push_back(f[2]);
}
void push3(std::vector<float>& v, size_t const f[3])
{
    v.push_back(f[0]);
    v.push_back(f[1]);
    v.push_back(f[2]);
}
std::vector<float> naive_mesh(size_t chunk[chunk_size][chunk_size][chunk_size])
{
    std::vector<float> vertices;
    for (size_t x = 0; x < chunk_size; ++x)
        for (size_t y = 0; y < chunk_size; ++y)
            for (size_t z = 0; z < chunk_size; ++z) {
                size_t c = chunk[x][y][z];
                if (c != 0)
                    for (size_t f = 0; f < 6; ++f)
                        for (size_t v = 0; v < 6; ++v) {
                            vertices.push_back(x + cube[f][v][0]);
                            vertices.push_back(y + cube[f][v][1]);
                            vertices.push_back(z + cube[f][v][2]);
                            push3(vertices, colors[c - 1]);
                        }
            }
    return vertices;
}
bool valid(size_t x, size_t y, size_t z)
{
    return (x < chunk_size && y < chunk_size && z < chunk_size);
}
std::vector<float> inner_removed_mesh(size_t chunk[chunk_size][chunk_size][chunk_size])
{
    bool is_closed[chunk_size][chunk_size][chunk_size][6] = {};
    for (size_t x = 0; x < chunk_size; ++x)
        for (size_t y = 0; y < chunk_size; ++y)
            for (size_t z = 0; z < chunk_size; ++z)
                if (chunk[x][y][z] != 0) {
                    if (valid(x + 1, y, z) && chunk[x + 1][y][z] != 0)
                        is_closed[x][y][z][0] = true;
                    if (valid(x - 1, y, z) && chunk[x - 1][y][z] != 0)
                        is_closed[x][y][z][1] = true;
                    if (valid(x, y + 1, z) && chunk[x][y + 1][z] != 0)
                        is_closed[x][y][z][2] = true;
                    if (valid(x, y - 1, z) && chunk[x][y - 1][z] != 0)
                        is_closed[x][y][z][3] = true;
                    if (valid(x, y, z + 1) && chunk[x][y][z + 1] != 0)
                        is_closed[x][y][z][4] = true;
                    if (valid(x, y, z - 1) && chunk[x][y][z - 1] != 0)
                        is_closed[x][y][z][5] = true;
                }

    std::vector<float> vertices;
    for (size_t x = 0; x < chunk_size; ++x)
        for (size_t y = 0; y < chunk_size; ++y)
            for (size_t z = 0; z < chunk_size; ++z) {
                size_t c = chunk[x][y][z];
                if (c != 0)
                    for (size_t f = 0; f < 6; ++f)
                        if (!is_closed[x][y][z][f])
                            for (size_t v = 0; v < 6; ++v) {
                                vertices.push_back(x + cube[f][v][0]);
                                vertices.push_back(y + cube[f][v][1]);
                                vertices.push_back(z + cube[f][v][2]);
                                push3(vertices, colors[c - 1]);
                            }
            }
    return vertices;
}
struct Point {
    size_t w[3];
    Point(size_t x, size_t y, size_t z)
        : w { x, y, z } { }
    Point operator+(Point const& p) const
    {
        return Point(w[0] + p.w[0], w[1] + p.w[1], w[2] + p.w[2]);
    }
    bool operator==(Point const& p) const
    {
        return w[0] == p.w[0] && w[1] == p.w[1] && w[2] == p.w[2];
    }
};
// static std::ostream& operator<<(std::ostream& o, Point p)
// {
//     return o << '(' << p.w[0] << ',' << p.w[1] << ',' << p.w[2] << ')';
// }
std::vector<float> greedy_mesh(size_t chunk[chunk_size][chunk_size][chunk_size])
{
    struct Quad {
        Point p[4];
        size_t c;
        std::vector<Point> pts;
        Quad(Point p1, Point p2, Point p3, Point p4)
            : p { p1, p2, p3, p4 }, c(0), pts {} { }
        Quad disp(Point q) const
        {
            return Quad(p[0] + q, p[1] + q, p[2] + q, p[3] + q);
        }
    };

    bool is_closed[chunk_size][chunk_size][chunk_size][6] = {};
    for (size_t x = 0; x < chunk_size; ++x)
        for (size_t y = 0; y < chunk_size; ++y)
            for (size_t z = 0; z < chunk_size; ++z)
                if (chunk[x][y][z] != 0) {
                    if (valid(x + 1, y, z) && chunk[x + 1][y][z] != 0)
                        is_closed[x][y][z][0] = true;
                    if (valid(x - 1, y, z) && chunk[x - 1][y][z] != 0)
                        is_closed[x][y][z][1] = true;
                    if (valid(x, y + 1, z) && chunk[x][y + 1][z] != 0)
                        is_closed[x][y][z][2] = true;
                    if (valid(x, y - 1, z) && chunk[x][y - 1][z] != 0)
                        is_closed[x][y][z][3] = true;
                    if (valid(x, y, z + 1) && chunk[x][y][z + 1] != 0)
                        is_closed[x][y][z][4] = true;
                    if (valid(x, y, z - 1) && chunk[x][y][z - 1] != 0)
                        is_closed[x][y][z][5] = true;
                }

    static const Quad q[6] = {
        { { 1, 0, 0 }, { 1, 0, 1 }, { 1, 1, 1 }, { 1, 1, 0 } },
        { { 0, 0, 0 }, { 0, 0, 1 }, { 0, 1, 1 }, { 0, 1, 0 } },
        { { 0, 1, 0 }, { 1, 1, 0 }, { 1, 1, 1 }, { 0, 1, 1 } },
        { { 1, 0, 0 }, { 0, 0, 0 }, { 0, 0, 1 }, { 1, 0, 1 } },
        { { 0, 1, 1 }, { 0, 0, 1 }, { 1, 0, 1 }, { 1, 1, 1 } },
        { { 0, 0, 0 }, { 0, 1, 0 }, { 1, 1, 0 }, { 1, 0, 0 } },
    };

    Quad* quads[chunk_size][chunk_size][chunk_size][6] = {};
    for (size_t x = 0; x < chunk_size; ++x)
        for (size_t y = 0; y < chunk_size; ++y)
            for (size_t z = 0; z < chunk_size; ++z) {
                size_t c = chunk[x][y][z];
                for (size_t f = 0; f < 6; ++f)
                    if (c != 0 && !is_closed[x][y][z][f]) {
                        Quad* d = new Quad(q[f].disp(Point { x, y, z }));
                        d->c = c;
                        d->pts.push_back(Point { x, y, z });
                        quads[x][y][z][f] = d;
                    }
            }

    static const Point d[2][6] = {
        {
            { 0, 1, 0 },
            { 0, 1, 0 },
            { 0, 0, 1 },
            { 0, 0, 1 },
            { 1, 0, 0 },
            { 1, 0, 0 },
        },
        {
            { 0, 0, 1 },
            { 0, 0, 1 },
            { 1, 0, 0 },
            { 1, 0, 0 },
            { 0, 1, 0 },
            { 0, 1, 0 },
        }
    };

    for (size_t dirn = 0; dirn < 2; ++dirn)
        for (size_t f = 0; f < 6; ++f) {
            for (size_t x = 0; x < chunk_size; ++x)
                for (size_t y = 0; y < chunk_size; ++y)
                    for (size_t z = 0; z < chunk_size; ++z) {
                        Quad* q = quads[x][y][z][f];
                        if (q != nullptr) {
                            size_t xn = x + d[dirn][f].w[0];
                            size_t yn = y + d[dirn][f].w[1];
                            size_t zn = z + d[dirn][f].w[2];
                            Quad* n;
                            if (valid(xn, yn, zn) && (n = quads[xn][yn][zn][f]) != nullptr && q->c == n->c) {
                                for (size_t e = 0; e < 4; ++e) {
                                    if (q->p[e] == n->p[(e + 3) & 3] && q->p[(e + 1) & 3] == n->p[(e + 2) & 3]) {
                                        q->p[e] = n->p[e];
                                        q->p[(e + 1) & 3] = n->p[(e + 1) & 3];
                                        for (Point p : n->pts)
                                            quads[p.w[0]][p.w[1]][p.w[2]][f] = q;
                                        q->pts.insert(q->pts.end(), n->pts.begin(), n->pts.end());
                                        delete n;
                                        break;
                                    }
                                }
                            }
                        }
                    }
        }

    std::vector<float> vertices;
    for (size_t x = 0; x < chunk_size; ++x)
        for (size_t y = 0; y < chunk_size; ++y)
            for (size_t z = 0; z < chunk_size; ++z) {
                size_t c = chunk[x][y][z];
                if (c != 0)
                    for (size_t f = 0; f < 6; ++f) {
                        Quad* q = quads[x][y][z][f];
                        if (q != nullptr) {
                            c = q->c;
                            push3(vertices, q->p[0].w);
                            push3(vertices, colors[c - 1]);
                            push3(vertices, q->p[2].w);
                            push3(vertices, colors[c - 1]);
                            push3(vertices, q->p[3].w);
                            push3(vertices, colors[c - 1]);
                            push3(vertices, q->p[0].w);
                            push3(vertices, colors[c - 1]);
                            push3(vertices, q->p[1].w);
                            push3(vertices, colors[c - 1]);
                            push3(vertices, q->p[2].w);
                            push3(vertices, colors[c - 1]);
                            for (Point p : q->pts)
                                quads[p.w[0]][p.w[1]][p.w[2]][f] = nullptr;
                            delete q;
                        }
                    }
            }
    return vertices;
}

int main()
{
    std::shared_ptr<Context> context = Context::create();

    std::unique_ptr<Window> window = Window::create(context, Window::Prop { "graphics", 1000, 1000 });
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

    std::shared_ptr<Shader> shader = Shader::create({ "res/shaders/vert.glsl", "res/shaders/frag.glsl" });
    shader->bind();

    std::ifstream f(chunk_file_path);
    size_t chunk[chunk_size][chunk_size][chunk_size];
    for (size_t x = 0; x < chunk_size; ++x)
        for (size_t y = 0; y < chunk_size; ++y)
            for (size_t z = 0; z < chunk_size; ++z)
                f >> chunk[x][y][z];
    // std::vector<float> vertices = naive_mesh(chunk);
    // std::vector<float> vertices = inner_removed_mesh(chunk);
    std::vector<float> vertices = greedy_mesh(chunk);
    size_t vertex_count = vertices.size() / 6;

    std::shared_ptr<VertexBuffer> vbo = VertexBuffer::create(&vertices[0], sizeof(vertices[0]) * vertices.size());
    vbo->set_layout({ { ShaderDataType::Float3, "aPos" }, { ShaderDataType::Float3, "aColor" } });

    std::shared_ptr<VertexArray> vao = VertexArray::create();
    vao->bind();

    vao->add_vertex_buffer(vbo);

    renderer->set_clear_color({ 0.2f, 0.3f, 0.3f, 1.0f });

    double start = window->get_time();
    size_t nr_frames = 0;
    window->init_delta_timer();
    while (!window_should_close) {
        renderer->clear();
        shader->set_mat4("projection", camera.projection(window->width * 1.0f / window->height));
        shader->set_mat4("view", camera.view());
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
    return 0;
}
