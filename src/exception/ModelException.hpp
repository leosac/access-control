/*
    Copyright (C) 2014-2016 Leosac

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

#pragma once

#include "exception/leosacexception.hpp"
#include <json.hpp>

/**
 * An exception class for general API error.
 *
 * The particularity of this class is that it accepts a JSON object
 * as a parameters.
 *
 * The ModelException accepts ModelException::ModelError objects as parameter.
 */
class ModelException : public LEOSACException
{
  public:
    using json = nlohmann::json;

    /**
     * Describes a model error.
     * This maps to DS.InvalidError on the client side (EmberJS).
     *
     * @note If `source_pointer` is "data", this means its a global model
     * error, not tied to a specific attribute.
     */
    struct ModelError
    {
        /**
         * The "path" to the source of the error.
         * Following JSON API, this is generally something like
         * "data/attributes/AttributeName"
         */
        std::string source_pointer;

        /**
         * The error message.
         */
        std::string message;
    };

    /**
     * This is an helper constructor that will instanciate a ModelError
     * object from the 2 parameters.
     *
     * @param source_pointer
     * @param msg
     */
    ModelException(const std::string &source_pointer, const std::string &msg);

    ModelException(const std::initializer_list<ModelError> &errors);

    /**
     * Format the ModelError object(s).
     */
    json json_errors() const;

    /**
     * Access the vector of errors.
     *
     * Can be useful to analyze errors precisely.
     */
    const std::vector<ModelError> errors() const;

  private:
    std::vector<ModelError> errors_;
};