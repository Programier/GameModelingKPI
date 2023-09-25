#include <SDL2/SDL_events.h>
#include <figure.hpp>
#include <iostream>
#include <object.hpp>
#include <properties.hpp>


Property::~Property() = default;

struct AProp : Property {
    const char* name() override
    {
        return "A";
    }

    float max_value() override
    {
        return 50;
    }

    float min_value() override
    {
        float res = glm::max(properties.figure_prop(PROP_INDEX_B),
                             properties.figure_prop(PROP_INDEX_C) + properties.figure_prop(PROP_INDEX_G));
        return res;
    }
};

struct BProp : Property {
    const char* name() override
    {
        return "B";
    }

    float max_value() override
    {
        return properties.figure_prop(PROP_INDEX_A);
    }

    float min_value() override
    {
        return 0.f;
    }
};

struct CProp : Property {
    const char* name() override
    {
        return "C";
    }

    float max_value() override
    {
        return properties.figure_prop(PROP_INDEX_A) - properties.figure_prop(PROP_INDEX_G);
    }

    float min_value() override
    {
        return properties.figure_prop(PROP_INDEX_G);
    }
};

struct DProp : Property {
    const char* name() override
    {
        return "D";
    }

    float max_value() override
    {
        return properties.figure_prop(PROP_INDEX_E) - properties.figure_prop(PROP_INDEX_G) / 2.0f;
    }

    float min_value() override
    {
        return 0.f;
    }
};

struct EProp : Property {
    const char* name() override
    {
        return "E";
    }

    float max_value() override
    {
        return properties.figure_prop(PROP_INDEX_F) - properties.figure_prop(PROP_INDEX_G) / 2.0f;
    }

    float min_value() override
    {
        return properties.figure_prop(PROP_INDEX_D) + properties.figure_prop(PROP_INDEX_G) / 2.0f;
    }
};

struct FProp : Property {
    const char* name() override
    {
        return "F";
    }

    float max_value() override
    {
        return 50;
    }

    float min_value() override
    {
        return properties.figure_prop(PROP_INDEX_E) + properties.figure_prop(PROP_INDEX_G) / 2.0f;
    }
};

struct GProp : Property {
    const char* name() override
    {
        return "G";
    }

    float max_value() override
    {
        float value = properties.figure_prop(PROP_INDEX_C);
        value       = glm::min(value, properties.figure_prop(PROP_INDEX_A) - properties.figure_prop(PROP_INDEX_C));
        value = glm::min(value, (properties.figure_prop(PROP_INDEX_E) - properties.figure_prop(PROP_INDEX_D)) * 2);
        value = glm::min(value, (properties.figure_prop(PROP_INDEX_F) - properties.figure_prop(PROP_INDEX_E)) * 2);
        return value;
    }

    float min_value() override
    {
        return properties.figure_prop(PROP_INDEX_H);
    }
};

struct HProp : Property {
    const char* name() override
    {
        return "H";
    }

    float max_value() override
    {
        return properties.figure_prop(PROP_INDEX_G);
    }

    float min_value() override
    {
        return 0.f;
    }
};

Properties::Properties()
{
    rendering.line_width   = 2.0f;
    rendering.point1_color = glm::vec3(1.0f);
    rendering.point2_color = glm::vec3(1.0f);

    grid.start_point = glm::ivec2(VIEWPORT_WIDTH / 2, VIEWPORT_HEIGHT / 2);
    grid.spacing     = DEFAULT_GRID_SPACING;
    grid.h_color1    = glm::vec3(0.5f);
    grid.h_color2    = glm::vec3(0.5f);
    grid.v_color1    = glm::vec3(0.5f);
    grid.v_color2    = glm::vec3(0.5f);

    grid.disable_axis = false;
    grid.disable_grid = false;

    figure.A = new AProp();
    figure.B = new BProp();
    figure.C = new CProp();
    figure.D = new DProp();
    figure.E = new EProp();
    figure.F = new FProp();
    figure.G = new GProp();
    figure.H = new HProp();

    figure.A->value = PROP_A / GRID_UNIT_SEGMENT;
    figure.B->value = PROP_B / GRID_UNIT_SEGMENT;
    figure.C->value = PROP_C / GRID_UNIT_SEGMENT;
    figure.D->value = PROP_D / GRID_UNIT_SEGMENT;
    figure.E->value = PROP_E / GRID_UNIT_SEGMENT;
    figure.F->value = PROP_F / GRID_UNIT_SEGMENT;
    figure.G->value = PROP_G / GRID_UNIT_SEGMENT;
    figure.H->value = PROP_H / GRID_UNIT_SEGMENT;

    if (!Figure::instance().build())
    {
        std::clog << "Failed to build figure!" << std::endl;
    }

    figure.color = glm::vec3(1.0, 1.0, 0.0);
}

float Properties::figure_prop(std::size_t index) const
{
    if (index < FIGURE_PROPS_COUNT)
    {
        return figure.props[index]->value;
    }

    return 0.0f;
}

Properties::~Properties()
{
    for (Property* prop : figure.props)
    {
        delete prop;
    }
}

Properties properties;


struct PropertiesUpdater : public Object {

    bool right_mouse_button_pressed = false;

    virtual void process_event(SDL_Event* event)
    {
        if (event->type == SDL_MOUSEBUTTONDOWN)
        {
            if (event->button.button == SDL_BUTTON_RIGHT)
            {
                right_mouse_button_pressed = true;
            }
        }
        else if (event->type == SDL_MOUSEBUTTONUP)
        {
            if (event->button.button == SDL_BUTTON_RIGHT)
            {
                right_mouse_button_pressed = false;
            }
        }
        else if (event->type == SDL_MOUSEMOTION)
        {
            static glm::ivec2 prev_pos;

            glm::ivec2 current = glm::ivec2(event->motion.x, -event->motion.y);

            if (right_mouse_button_pressed)
            {
                glm::ivec2 diff = current - prev_pos;
                properties.grid.start_point += diff;
            }

            prev_pos = current;
        }
        else if (event->type == SDL_MOUSEWHEEL)
        {
            properties.grid.spacing =
                    glm::max(properties.grid.spacing + (event->wheel.y * glm::max(properties.grid.spacing / 10, 1)), 1);
        }
    }
} updater;
