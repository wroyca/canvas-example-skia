#pragma once

#include <iosfwd>
#include <string>

#include <libcanvas/export.hxx>

namespace canvas
{
  // Print a greeting for the specified name into the specified
  // stream. Throw std::invalid_argument if the name is empty.
  //
  LIBCANVAS_SYMEXPORT void
  say_hello (std::ostream&, const std::string& name);
}
