/*             DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
                    Version 2, December 2004

 Copyright (C) 2004 Sam Hocevar <sam@hocevar.net>

 Everyone is permitted to copy and distribute verbatim or modified
 copies of this license document, and changing it is allowed as long
 as the name is changed.

            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

  0. You just DO WHAT THE FUCK YOU WANT TO.
*/

// My signature SVG graphic is copyrighted. If you reuse my components, remove my signature.
//
// Other than that, this widget library is unencumbered by the licensing restrictions of
// VCV's component library, as it doesn't use its graphics or call its code.
// 
// Widgets are only added to my library as the need arises. If I'm not using a specific variant,
// I do not create it until I need it.
//
// If you want to re-use one of my one-off widgets that is not in this file but in a module, thus
// covered by the GPL, and wish to receive the code of that widget under the WTFPL, contact me.



#pragma once
using namespace rack;
extern Plugin* pluginInstance;


namespace W { // I don't want to type MyCoolPersonalWidgets:: every damn time, thank you



/* --------------------------------------------------------------------------------------------- */
/* ---- Base ----------------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------------------------- */

// All the features, none of the shade
struct SvgSwitchUnshadowed : SvgSwitch {
    SvgSwitchUnshadowed() {
        fb = new widget::FramebufferWidget;
        addChild(fb);

        sw = new widget::SvgWidget;
        fb->addChild(sw);
    }
};



/* --------------------------------------------------------------------------------------------- */
/* ---- Lights --------------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------------------------- */

// Values are kinda yolo'd by trial and error here.
// We don't want a halo - they're too visible on my faceplates, and slated for removal in VCV 2.0 anyway


// Those lights are used for dynamic jacks.
// They must be added before transparent jacks, at the same position (use the helper).
// They are cut off in the middle for Lights Off compatibility.
struct JackLight : app::ModuleLightWidget {
    JackLight() {
        this->box.size = app::mm2px(math::Vec(8.0, 8.0));
        this->bgColor = nvgRGB(0x0e, 0x69, 0x77);
    }
    
    void draw(const widget::Widget::DrawArgs& args) override {
        float radius = std::min(this->box.size.x, this->box.size.y) / 2.0 - 0.5f;
        float holeRadius = app::mm2px(3.f);

        nvgBeginPath(args.vg);
        nvgCircle(args.vg, radius + 1.f, radius + 1.f, radius);
        nvgCircle(args.vg, radius + 1.f, radius + 1.f, holeRadius);
        nvgPathWinding(args.vg, NVG_HOLE);

        // Background
        if (this->bgColor.a > 0.0) {
            nvgFillColor(args.vg, this->bgColor);
            nvgFill(args.vg);
        }

        // Foreground
        if (this->color.a > 0.0) {
            nvgFillColor(args.vg, this->color);
            nvgFill(args.vg);
        }
    }

};


struct JackLightInput : JackLight {
    JackLightInput() {
        this->addBaseColor(nvgRGB(0xff, 0xcc, 0x03));
    }
};


struct JackLightOutput : JackLight {
    JackLightOutput() {
        this->addBaseColor(nvgRGB(0xfc, 0xae, 0xbb));
    }
};


// Those lights are used for static jacks.
// They must be added before transparent jacks, at the same position (use the helper).
// They are cut off in the middle for Lights Off compatibility.
struct JackStaticLight : app::LightWidget {
    JackStaticLight() {
        this->box.size = app::mm2px(math::Vec(8.0, 8.0));
    }
    
    void draw(const widget::Widget::DrawArgs& args) override {
        float radius = std::min(this->box.size.x, this->box.size.y) / 2.0 - 0.5f;
        float holeRadius = app::mm2px(3.f);

        nvgBeginPath(args.vg);
        nvgCircle(args.vg, radius + 1.f, radius + 1.f, radius);
        nvgCircle(args.vg, radius + 1.f, radius + 1.f, holeRadius);
        nvgPathWinding(args.vg, NVG_HOLE);

        // Foreground
        nvgFillColor(args.vg, this->color);
        nvgFill(args.vg);
    }

};

struct JackStaticLightInput : JackStaticLight {
    JackStaticLightInput() {
        this->color = nvgRGB(0xff, 0xcc, 0x03);
    }
};

struct JackStaticLightOutput : JackStaticLight {
    JackStaticLightOutput() {
        this->color = nvgRGB(0xfc, 0xae, 0xbb);
    }
};


// Those lights must be added before transparent knobs, at the same position.
// If given a ParamQuantity pointer, draws a little dark segment for Lights Off mode.
struct KnobLight : ModuleLightWidget {
    ParamQuantity* paramQuantity = NULL;
    float min = 0.f;
    float max = 10.f;

    KnobLight() {
        this->box.size = app::mm2px(math::Vec(8.0f, 8.0f));
        this->bgColor = nvgRGB(0x0e, 0x69, 0x77);
    }
    
    void draw(const widget::Widget::DrawArgs& args) override {
        float radius = std::min(this->box.size.x, this->box.size.y) / 2.0 - 2.6f;

        nvgBeginPath(args.vg);
        nvgCircle(args.vg, radius + 2.6f, radius + 2.6f, radius);

        // Background
        if (this->bgColor.a > 0.0) {
            nvgFillColor(args.vg, this->bgColor);
            nvgFill(args.vg);
        }

        // Foreground
        if (this->color.a > 0.0) {
            nvgFillColor(args.vg, this->color);
            nvgFill(args.vg);
        }

        // Draw a dark segment to show the position of the knob, for Lights Off support. 
        if (module && paramQuantity) {
            nvgBeginPath(args.vg);
            nvgMoveTo(args.vg, mm2px(4.f), mm2px(4.f));
            // Rotates by -90 degrees in radians
            float value = rescale(paramQuantity->getValue(), min, max, -0.83f * M_PI - 1.570796f, 0.83f * M_PI - 1.570796f);
            float targetX = mm2px(4.f + 3.2f * cos(value));
            float targetY = mm2px(4.f + 3.2f * sin(value));
            nvgLineTo(args.vg, targetX, targetY);
            nvgStrokeColor(args.vg, nvgRGB(0x33, 0x00, 0x00));
            nvgStrokeWidth(args.vg, 2.f);
            nvgStroke(args.vg);
        }        
    }

};

// Helper to create a KnobLight that goes below the knob, with a little dark segment for Lights Off mode.
template <class TKnobLight>
TKnobLight* createKnobLight(math::Vec pos, Module* module, int lightId, int paramId, float min, float max) {
    TKnobLight* o = new TKnobLight;
    o->box.pos = pos;
    o->module = module;
    o->firstLightId = lightId;
    if (module) o->paramQuantity = module->paramQuantities[paramId];
    o->min = min;
    o->max = max;
    return o;
}

struct KnobLightYellow : KnobLight {
    KnobLightYellow() {
        this->addBaseColor(nvgRGB(0xff, 0xcc, 0x03));
    }
};


// Tiny little status lights. 2.17mm
struct StatusLight : ModuleLightWidget {
    StatusLight() {
        this->box.size = app::mm2px(math::Vec(2.176f, 2.176f));
        this->bgColor = nvgRGB(0x0e, 0x69, 0x77);
        this->borderColor = nvgRGB(0x08, 0x3d, 0x45);
    }
    
    void draw(const widget::Widget::DrawArgs& args) override {
        float radius = std::min(this->box.size.x, this->box.size.y) / 2.0 - 0.5f;

        nvgBeginPath(args.vg);
        nvgCircle(args.vg, radius, radius , radius);

        // Background
        if (this->bgColor.a > 0.0) {
            nvgFillColor(args.vg, this->bgColor);
            nvgFill(args.vg);
        }

        // Foreground
        if (this->color.a > 0.0) {
            nvgFillColor(args.vg, this->color);
            nvgFill(args.vg);
        }

        // Border
        if (this->borderColor.a > 0.0) {
            nvgStrokeWidth(args.vg, app::mm2px(0.2));
            nvgStrokeColor(args.vg, this->borderColor);
            nvgStroke(args.vg);
        }
    }

};


// 2.17mm
struct StatusLightOutput : StatusLight {
    StatusLightOutput() {
        this->addBaseColor(nvgRGB(0xfc, 0xae, 0xbb));
    }
};


// 2.17mm
struct StatusLightInput : StatusLight {
    StatusLightInput() {
        this->addBaseColor(nvgRGB(0xff, 0xcc, 0x03));
    }
};




/* --------------------------------------------------------------------------------------------- */
/* ---- Jacks ---------------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------------------------- */

// TODO: Make all my jacks use a light widget, including the non-dynamic ones, for Lights Off consistency.

// Base jack is a SVGPort without customizations.
struct Jack : SVGPort {

};


// Transparent jacks are shown above a light.
struct JackTransparent : Jack {
    JackTransparent() {
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/jack-transparent.svg")));
        Jack();
    }
};


// Helper to create a lit input comprised of a LED and a transparent Jack. The light is dynamic.
inline Widget* createLitInput(math::Vec pos, engine::Module* module, int inputId, int firstLightId) {
	Widget* o = new Widget;
    JackLightInput* light = new JackLightInput;
    JackTransparent* jack = new JackTransparent;

	light->module = module;
	light->firstLightId = firstLightId;

	jack->module = module;
	jack->type = app::PortWidget::INPUT;
	jack->portId = inputId;

    o->box.pos = pos;
    o->addChild(light);
    o->addChild(jack);
	return o;
}


// Helper to create a lit output comprised of a LED and a transparent Jack. The light is dynamic.
inline Widget* createLitOutput(math::Vec pos, engine::Module* module, int outputId, int firstLightId) {
	Widget* o = new Widget;
    JackLightOutput* light = new JackLightOutput;
    JackTransparent* jack = new JackTransparent;

	light->module = module;
	light->firstLightId = firstLightId;

	jack->module = module;
	jack->type = app::PortWidget::OUTPUT;
	jack->portId = outputId;

    o->box.pos = pos;
    o->addChild(light);
    o->addChild(jack);
	return o;
}


// Helper to create an input comprised of a LED and a transparent Jack. The light is constantly lit.
inline Widget* createInput(math::Vec pos, engine::Module* module, int inputId) {
	Widget* o = new Widget;
    JackStaticLightInput* light = new JackStaticLightInput;
    JackTransparent* jack = new JackTransparent;

	jack->module = module;
	jack->type = app::PortWidget::INPUT;
	jack->portId = inputId;

    o->box.pos = pos;
    o->addChild(light);
    o->addChild(jack);
	return o;
}

// Helper to create an output comprised of a LED and a transparent Jack. The light is constantly lit.
inline Widget* createOutput(math::Vec pos, engine::Module* module, int inputId) {
	Widget* o = new Widget;
    JackStaticLightOutput* light = new JackStaticLightOutput;
    JackTransparent* jack = new JackTransparent;

	jack->module = module;
	jack->type = app::PortWidget::OUTPUT;
	jack->portId = inputId;

    o->box.pos = pos;
    o->addChild(light);
    o->addChild(jack);
	return o;
}


/* --------------------------------------------------------------------------------------------- */
/* ---- Switches ------------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------------------------- */

// ------------------------- Pushbuttons ----------------------------------------------------------

// 5mm
struct SmallButton : SvgSwitch {
    SmallButton() {
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/pushbutton-500-off.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/pushbutton-500-on.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/pushbutton-500-pink.svg")));
    }
};


// 5mm
struct SmallButtonMomentary : SvgSwitch {
    SmallButtonMomentary() {
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/pushbutton-500-off.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/pushbutton-500-on.svg")));
        momentary = true;
    }
};


// 7mm
struct ReducedButton : SvgSwitch {
    ReducedButton() {
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/pushbutton-700-off.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/pushbutton-700-on.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/pushbutton-700-pink.svg")));
    }
};


// 8.20mm.
struct Button : SvgSwitch {
    Button() {
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/pushbutton-820-off.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/pushbutton-820-on.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/pushbutton-820-pink.svg")));
    }
};


// 8.20mm.
struct ButtonMomentary : SvgSwitch {
    ButtonMomentary() {
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/pushbutton-820-off.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/pushbutton-820-on.svg")));
        momentary = true;
    }
};


// 8.20mm. You won't guess its color when you press it.
struct ButtonPink : SvgSwitch {
    ButtonPink() {
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/pushbutton-820-off.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/pushbutton-820-pink.svg")));
    }
};



// ------------------------- Rocker switches ------------------------------------------------------


// Rocker siwtch, horizontal. Left is default
struct RockerSwitchHorizontal : SvgSwitchUnshadowed {
    RockerSwitchHorizontal() {
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/rocker-switch-800-l.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/rocker-switch-800-r.svg")));
    }
};


// Rocker siwtch, vertical. Up is default
struct RockerSwitchVertical : SvgSwitchUnshadowed {
    RockerSwitchVertical() {
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/rocker-switch-800-u.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/rocker-switch-800-d.svg")));
    }
};





/* --------------------------------------------------------------------------------------------- */
/* ---- Knobs ---------------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------------------------- */


// 8.2mm
struct Knob : app::SvgKnob {
    Knob() {
        minAngle = -0.83 * M_PI;
        maxAngle = 0.83 * M_PI;
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/knob-820.svg")));
    }
};

// 8.2mm
struct KnobSnap : Knob {
    KnobSnap() {
        snap = true;
        Knob();
    }
};

// 8.2mm
struct KnobTransparent : Knob {
    KnobTransparent() {
        minAngle = -0.83 * M_PI;
        maxAngle = 0.83 * M_PI;
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/knob-820-transparent.svg")));
    }
};




/* --------------------------------------------------------------------------------------------- */
/* ---- Decorative ----------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------------------------- */

// These require a standard <3-shaped screwdriver, provided complimentary with every purchasee. 
struct Screw : SvgScrew {
    Screw() {
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/screw.svg")));
    }
};


// My personal brand, featuring the Cool S. Standard vertical position is 114.5mm.
// Using a SvgScrew for the handy built-in framebuffer.
// If you reuse these components, change the corresponding SVG file. Do not reuse my signature in your own works.
// See the README for full legal details. 
struct Signature : SvgScrew {
    Signature() {
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/signature/signature.svg")));
    }
};





} // namespace W
