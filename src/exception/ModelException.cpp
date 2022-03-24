/*
    Copyright (C) 2014-2022 Leosac

    This file is part of Leosac.

    Leosac is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Leosac is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "exception/ModelException.hpp"
#include "tools/log.hpp"

ModelException::ModelException(const std::string &source_pointer,
                               const std::string &msg)
    : LEOSACException("ModelException.")
{
    ModelError e;
    e.source_pointer = source_pointer;
    e.message        = msg;
    errors_.push_back(e);

    message_ = BUILD_STR("ModelException: " << json_errors().dump(4));
}

ModelException::ModelException(
    const std::initializer_list<ModelException::ModelError> &errors)
    : LEOSACException("ModelException.")
    , errors_(errors)
{
    message_ = BUILD_STR("ModelException: " << json_errors().dump(4));
}

ModelException::json ModelException::json_errors() const
{
    json json_errors = json::array();
    for (const auto &error : errors_)
    {
        json json_error;
        json_error["detail"]            = error.message;
        json_error["source"]["pointer"] = error.source_pointer;
        json_errors.push_back(json_error);
    }

    return json_errors;
}

const std::vector<ModelException::ModelError> ModelException::errors() const
{
    return errors_;
}
