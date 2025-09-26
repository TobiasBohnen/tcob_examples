// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "LSystemEx.hpp"
#include "tcob/core/Color.hpp"
#include <iomanip>
#include <iostream>

#include "../_common/Trim.hpp"

using namespace std::chrono_literals;

LSystemEx::LSystemEx(game& game)
    : scene {game}
{
    window().ClearColor = colors::Green;
}

LSystemEx::~LSystemEx() = default;

void LSystemEx::on_fixed_update(milliseconds /* deltaTime */)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    auto const& stats {locate_service<gfx::render_system>().statistics()};
    stream << "avg FPS:" << stats.average_FPS();
    stream << " best FPS:" << stats.best_FPS();
    stream << " worst FPS:" << stats.worst_FPS();

    window().Title = "TestGame " + stream.str();
}

void LSystemEx::on_start()
{
    auto&     win {window()};
    auto      windowSize {*win.Size};
    i32 const uiWidth {static_cast<i32>(windowSize.Height / 2)};

    rect_i const menuBounds {windowSize.Width - uiWidth, 0, uiWidth, windowSize.Height};
    _mainForm = std::make_shared<lsystem_form>(menuBounds);
    _mainForm->Start.connect([this, windowSize, uiWidth] {
        auto const settings {_mainForm->get_settings()};
        _lstring    = settings.String;
        _lstringpos = 0;
        _countdown  = 500ms;

        _currentState.Position     = {(windowSize.Width - uiWidth) / 2.0f, windowSize.Height * 0.95f};
        _currentState.Angle        = degree_f {0};
        _currentState.LineLength   = settings.LineLength;
        _currentState.TurningAngle = settings.TurningAngle;
        _currentState.StrokeWidth  = settings.StrokeWidth;
        _currentState.SwapTurn     = false;
        _stateStack.clear();

        _canvas.begin_frame(windowSize, 1);
        _canvas.end_frame();
    });
    root_node()->Entity = _mainForm;

    _canvas.begin_frame(windowSize, 1);
    _canvas.end_frame();
}

void LSystemEx::on_update(milliseconds deltaTime)
{
    if (_lstringpos < _lstring.size()) {
        _countdown -= deltaTime;
        if (_countdown <= 0s) { step(); }
    }
}

void LSystemEx::on_draw_to(render_target& target)
{
    _renderer.add_layer(0);
    _renderer.set_bounds({point_f::Zero, size_f {*window().Size}});
    _renderer.render_to_target(target);
}

void LSystemEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) { // NOLINT
    case scan_code::R: {
        auto _ = window().copy_to_image().save("screen01.webp");
    } break;
    case scan_code::BACKSPACE:
        parent().pop_current_scene();
        break;
    default:
        break;
    }
}

void LSystemEx::step()
{
    _canvas.begin_frame(*window().Size, 1, 0, false);

    _canvas.set_edge_antialias(false);
    _canvas.set_stroke_style(colors::Black);
    _canvas.set_fill_style(colors::Red);
    _canvas.set_stroke_width(_currentState.StrokeWidth);

    f32 const      lineLengthScaleFactor {1.1f};
    degree_f const turningAngleIncrement {1.0f};
    f32 const      lineWidthIncrement {1.0f};

    switch (_lstring[_lstringpos++]) {
    case 'F': {
        // Move forward while drawing.
        i32 count {1};
        while (_lstring[_lstringpos] == 'F') {
            ++count;
            ++_lstringpos;
        }
        _canvas.begin_path();
        _canvas.move_to(_currentState.Position);
        _currentState.Position += point_f::FromDirection(_currentState.Angle) * _currentState.LineLength * count;
        _canvas.line_to(_currentState.Position);
        _canvas.stroke();
        _countdown = 50ms;
        break;
    }
    case 'f': {
        // Move forward without drawing.
        _currentState.Position += point_f::FromDirection(_currentState.Angle) * _currentState.LineLength;
        break;
    }
    case '+': {
        // Turn left (or right if swapped).
        if (!_currentState.SwapTurn) {
            _currentState.Angle -= _currentState.TurningAngle;
        } else {
            _currentState.Angle += _currentState.TurningAngle;
        }
        break;
    }
    case '-': {
        // Turn right (or left if swapped).
        if (!_currentState.SwapTurn) {
            _currentState.Angle += _currentState.TurningAngle;
        } else {
            _currentState.Angle -= _currentState.TurningAngle;
        }
        break;
    }
    case '|': {
        // Reverse direction (turn 180°).
        _currentState.Angle += degree_f {180.0f};
        break;
    }
    case '[': {
        // Push the current state onto the stack.
        _stateStack.push_back(_currentState);
        break;
    }
    case ']': {
        // Pop the state from the stack.
        if (!_stateStack.empty()) {
            _currentState = _stateStack.back();
            _stateStack.pop_back();
        }
        break;
    }
    case '#': {
        // Increase the stroke width.
        _currentState.StrokeWidth += lineWidthIncrement;
        break;
    }
    case '!': {
        // Decrease the stroke width.
        _currentState.StrokeWidth = std::max(1.0f, _currentState.StrokeWidth - lineWidthIncrement);
        break;
    }
    case '@': {
        // Draw a dot at the current position.
        _canvas.begin_path();
        _canvas.circle(_currentState.Position, _currentState.StrokeWidth);
        _canvas.fill();
        _countdown = 125ms;
        break;
    }
    case '>': {
        // Increase the line length.
        _currentState.LineLength *= lineLengthScaleFactor;
        break;
    }
    case '<': {
        // Decrease the line length.
        _currentState.LineLength /= lineLengthScaleFactor;
        break;
    }
    case '&': {
        // Swap the meaning of '+' and '-'.
        _currentState.SwapTurn = !_currentState.SwapTurn;
        break;
    }
    case '(': {
        // Decrease the turning angle.
        _currentState.TurningAngle -= turningAngleIncrement;
        break;
    }
    case ')': {
        // Increase the turning angle.
        _currentState.TurningAngle += turningAngleIncrement;
        break;
    }
    default:
        // Ignore unrecognized characters.
        break;
    }

    _canvas.end_frame();
}
