/*  Copyright (C) 2019-2020 Aria Salvatrice
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

// Small controllers.
// Rotatoes = knobs
// Faders and buttons planned next.

#include "plugin.hpp"

namespace Controllers {

// Nope, no audio rate option provided until someone makes a strong argument why they need it.
const int PROCESSDIVIDER = 32;

template <size_t KNOBS>
struct Rotatoes : Module {
    enum ParamIds {
        ENUMS(ROTATO_PARAM, KNOBS),
        NUM_PARAMS
    };
    enum InputIds {
        EXT_SCALE_INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        ENUMS(CV_OUTPUT, KNOBS),
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };


    float min[KNOBS];
    float max[KNOBS];
    dsp::ClockDivider processDivider;
    

    Rotatoes() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        for(size_t i = 0; i < KNOBS; i++){
            configParam(ROTATO_PARAM + i, 0.f, 1.f, 0.f, "Rotato " + std::to_string(i + 1));
            min[i] = 0.f;
            max[i] = 10.f;
        }

        processDivider.setDivision(PROCESSDIVIDER);
    }


    json_t* dataToJson() override {
        json_t* rootJ = json_object();

        json_t *minJ = json_array();
        for (size_t i = 0; i < KNOBS; i++) json_array_insert_new(minJ, i, json_real(min[i]));
        json_object_set_new(rootJ, "min", minJ);
        
        json_t *maxJ = json_array();
        for (size_t i = 0; i < KNOBS; i++) json_array_insert_new(maxJ, i, json_real(max[i]));
        json_object_set_new(rootJ, "max", maxJ);

        return rootJ;
    }


    void dataFromJson(json_t* rootJ) override {
        json_t *minJ = json_object_get(rootJ, "min");
        if (minJ) {
            for (size_t i = 0; i < KNOBS; i++) {
                json_t *minValueJ = json_array_get(minJ, i);
                if (minValueJ) min[i] = json_real_value(minValueJ);
            }
        }
        json_t *maxJ = json_object_get(rootJ, "max");
        if (maxJ) {
            for (size_t i = 0; i < KNOBS; i++) {
                json_t *maxValueJ = json_array_get(maxJ, i);
                if (maxValueJ) max[i] = json_real_value(maxValueJ);
            }
        }
    }


    void process(const ProcessArgs& args) override {
        if (processDivider.process()) {
            for(size_t i = 0; i < KNOBS; i++) {
                outputs[CV_OUTPUT + i].setVoltage( rescale(params[ROTATO_PARAM + i].getValue(), 0.f, 1.f, min[i], max[i]) );
            }
        }
    }
};




///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////




// Add a margin to my normal knob, so the square that shows it's bound to MIDI is offset a bit.
// A black placeholder square is added to the faceplate. Positioning of the rectangle is yolo'd.
struct AriaKnobRotato : AriaKnob820 {
    AriaKnobRotato() {
        AriaKnob820();
        box.size.x += 4.f;
        box.size.y += 2.1f;
    }
};

// Code mostly copied from https://github.com/gluethegiant/gtg-rack/blob/master/src/gtgComponents.hpp
struct MinMaxQuantity : Quantity {
	float *voltage = NULL;
	std::string label = "";

	MinMaxQuantity(float *_voltage, std::string _label) {
		voltage = _voltage;
		label = _label;
	}
	void setValue(float value) override {
		*voltage = math::clamp(value, getMinValue(), getMaxValue());
	}
	float getValue() override {
		return *voltage;
	}
	float getMinValue() override { return -10.0f; }
	float getMaxValue() override { return 10.0f; }
	std::string getLabel() override { return label; }
	std::string getUnit() override { return " V"; }
};

template <size_t KNOBS>
struct RotatoSettingsItem : MenuItem {
    Rotatoes<KNOBS>* module;
    size_t knob;

    struct MinMaxSliderItem : ui::Slider {
        MinMaxSliderItem(float *voltage, std::string label) {
            quantity = new MinMaxQuantity(voltage, label);
        }
        ~MinMaxSliderItem() {
            delete quantity;
        }
    };

    struct RotatoSettingUnipolar : MenuItem {
        Rotatoes<KNOBS>* module;
        size_t knob;
        void onAction(const event::Action &e) override {
            module->min[knob] = 0.f;
            module->max[knob] = 10.f;
        }
    };

    struct RotatoSettingUnipolar5v : MenuItem {
        Rotatoes<KNOBS>* module;
        size_t knob;
        void onAction(const event::Action &e) override {
            module->min[knob] = 0.f;
            module->max[knob] = 5.f;
        }
    };

    struct RotatoSettingBipolar : MenuItem {
        Rotatoes<KNOBS>* module;
        size_t knob;
        void onAction(const event::Action &e) override {
            module->min[knob] = -5.f;
            module->max[knob] = 5.f;
        }
    };

    struct RotatoSettingUnipolarInverted : MenuItem {
        Rotatoes<KNOBS>* module;
        size_t knob;
        void onAction(const event::Action &e) override {
            module->min[knob] = 10.f;
            module->max[knob] = 0.f;
        }
    };

    struct RotatoSettingUnipolar5vInverted : MenuItem {
        Rotatoes<KNOBS>* module;
        size_t knob;
        void onAction(const event::Action &e) override {
            module->min[knob] = 5.f;
            module->max[knob] = 0.f;
        }
    };

    struct RotatoSettingBipolarInverted : MenuItem {
        Rotatoes<KNOBS>* module;
        size_t knob;
        void onAction(const event::Action &e) override {
            module->min[knob] = -5.f;
            module->max[knob] = 5.f;
        }
    };

    Menu *createChildMenu() override {
        Menu *menu = new Menu;

        menu->addChild(createMenuLabel("Range (can be inverted)"));

		MinMaxSliderItem *minSliderItem = new MinMaxSliderItem(&module->min[knob], "Minimum");
		minSliderItem->box.size.x = 190.f;
		menu->addChild(minSliderItem);

		MinMaxSliderItem *maxSliderItem = new MinMaxSliderItem(&module->max[knob], "Maximum");
		maxSliderItem->box.size.x = 190.f;
		menu->addChild(maxSliderItem);

        menu->addChild(new MenuSeparator());

        menu->addChild(createMenuLabel("Presets"));

        RotatoSettingUnipolar *rotatoSettingUnipolar = createMenuItem<RotatoSettingUnipolar>("Set to 0 V ~ 10 V", "");
        rotatoSettingUnipolar->module = module;
        rotatoSettingUnipolar->knob = knob;
        menu->addChild(rotatoSettingUnipolar);

        RotatoSettingUnipolar5v *rotatoSettingUnipolar5v = createMenuItem<RotatoSettingUnipolar5v>("Set to 0 V ~ 5 V", "");
        rotatoSettingUnipolar5v->module = module;
        rotatoSettingUnipolar5v->knob = knob;
        menu->addChild(rotatoSettingUnipolar5v);

        RotatoSettingBipolar *rotatoSettingBipolar = createMenuItem<RotatoSettingBipolar>("Set to -5 V ~ 5 V", "");
        rotatoSettingBipolar->module = module;
        rotatoSettingBipolar->knob = knob;
        menu->addChild(rotatoSettingBipolar);

        menu->addChild(createMenuLabel("Inverted Presets"));

        RotatoSettingUnipolarInverted *rotatoSettingUnipolarInverted = createMenuItem<RotatoSettingUnipolarInverted>("Set to 10 V ~ 0 V", "");
        rotatoSettingUnipolarInverted->module = module;
        rotatoSettingUnipolarInverted->knob = knob;
        menu->addChild(rotatoSettingUnipolarInverted);

        RotatoSettingUnipolar5vInverted *rotatoSettingUnipolar5vInverted = createMenuItem<RotatoSettingUnipolar5vInverted>("Set to 5 V ~ 0 V", "");
        rotatoSettingUnipolar5vInverted->module = module;
        rotatoSettingUnipolar5vInverted->knob = knob;
        menu->addChild(rotatoSettingUnipolar5vInverted);

        RotatoSettingBipolarInverted *rotatoSettingBipolarInverted = createMenuItem<RotatoSettingBipolarInverted>("Set to 5 V ~ -5 V", "");
        rotatoSettingBipolarInverted->module = module;
        rotatoSettingBipolarInverted->knob = knob;
        menu->addChild(rotatoSettingBipolarInverted);

        return menu;
    }

};


struct Rotatoes4Widget : ModuleWidget {

    void drawRotato(Rotatoes<4>* module, float y, int num) {
        addParam(createParam<AriaKnobRotato>(mm2px(Vec(3.52f, y)), module, Rotatoes<4>::ROTATO_PARAM + num));
        addOutput(createOutput<AriaJackOut>(mm2px(Vec(3.52f, y + 10.f)), module, Rotatoes<4>::CV_OUTPUT + num));
    }

    Rotatoes4Widget(Rotatoes<4>* module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/faceplates/Rotatoes.svg")));

        // Signature
        addChild(createWidget<AriaSignature>(mm2px(Vec(1.0f, 114.5f))));

        // External
        addInput(createInput<AriaJackIn>(mm2px(Vec(3.52f, 15.9f)), module, Rotatoes<4>::EXT_SCALE_INPUT));

        // Rotatoes
        drawRotato(module, 31.f, 0);
        drawRotato(module, 52.f, 1);
        drawRotato(module, 73.f, 2);
        drawRotato(module, 94.f, 3);

        // Screws
        addChild(createWidget<AriaScrew>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<AriaScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<AriaScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<AriaScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    }

    void appendContextMenu(ui::Menu *menu) override {	
        Rotatoes<4> *module = dynamic_cast<Rotatoes<4>*>(this->module);
        assert(module);

        menu->addChild(new MenuSeparator());

        for(size_t i = 0; i < 4; i++) {
            RotatoSettingsItem<4> *rotatoSettingsItem = createMenuItem<RotatoSettingsItem<4>>("Rotato " + std::to_string(i + 1), RIGHT_ARROW);
            rotatoSettingsItem->module = module;
            rotatoSettingsItem->knob = i;
            menu->addChild(rotatoSettingsItem);
        }

    }

};

} // Namespace Controllers

Model* modelRotatoes4 = createModel<Controllers::Rotatoes<4>, Controllers::Rotatoes4Widget>("Rotatoes4");
