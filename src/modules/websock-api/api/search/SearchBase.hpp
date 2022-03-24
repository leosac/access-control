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

#pragma once

#include "tools/db/DBService.hpp"
#include "tools/log.hpp"
#include <nlohmann/json.hpp>

namespace Leosac
{
namespace Module
{
namespace WebSockAPI
{

/**
 * Use this tag to search for entities with an "alias" field
 */
struct use_alias_tag
{
};
/**
 * Use this tag to search for entities with a "name"
 */
struct use_name_tag
{
};

/**
 * Use this tag to search for entities with a "username" tag
 * (Most likely only "User" for now ...
 */
struct use_username_tag
{
};

/**
 * This is a templated class that perform case-insensitive database
 * search against entities.
 *
 * It can be used to implemented the various `search.*` API endpoints.
 */
template <typename DatabaseEntity, typename AliasOrName>
struct EntitySearchTool
{

    using Query = odb::query<DatabaseEntity>;

    struct DatabaseEntityComparator
    {
        bool operator()(const DatabaseEntity &e1, const DatabaseEntity &e2)
        {
            static_assert(std::is_integral<decltype(e1.id())>::value,
                          "Id is not an integral type.");
            ASSERT_LOG(e1.id(), "entity1 has no id.");
            ASSERT_LOG(e2.id(), "entity2 has no id.");
            return e1.id() < e2.id();
        }
    };

    using EntitySet = std::set<DatabaseEntity, DatabaseEntityComparator>;

  private:
    template <typename T>
    std::enable_if_t<std::is_same<T, use_alias_tag>::value, Query>
    build_query(const std::string &partial)
    {
        Query q(Query::alias.like("%" + partial + "%"));
        return std::move(q);
    }

    template <typename T>
    std::enable_if_t<std::is_same<T, use_name_tag>::value, Query>
    build_query(const std::string &partial)
    {
        Query q(Query::name.like("%" + partial + "%"));
        return std::move(q);
    }

    template <typename T>
    std::enable_if_t<std::is_same<T, use_username_tag>::value, Query>
    build_query(const std::string &partial)
    {
        Query q(Query::username.like("%" + partial + "%"));
        return std::move(q);
    }

    void search_and_append(DBPtr db, EntitySet &entities, const std::string &partial)
    {
        Query q(build_query<AliasOrName>(partial));
        auto results = db->query(q);
        for (const auto &ap : results)
        {
            entities.insert(ap);
        }
    };

  public:
    std::vector<DatabaseEntity> search(DBPtr db,
                                       const std::string &partial_name_or_alias)
    {
        odb::transaction t(db->begin());
        EntitySet entities;

        search_and_append(db, entities, partial_name_or_alias);

        // Attempt to convert each letter to CAPS so the search is
        // a not too much case sensitive.
        // Todo: This needs to be improved.
        for (auto i = 0u; i < partial_name_or_alias.length(); ++i)
        {
            auto partial_name_copy = partial_name_or_alias;
            partial_name_copy[i]   = std::toupper(partial_name_or_alias[i]);
            search_and_append(db, entities, partial_name_copy);
        }

        return std::vector<DatabaseEntity>(entities.begin(), entities.end());
    }

  private:
    template <typename T>
    std::enable_if_t<std::is_same<T, use_alias_tag>::value, json>
    build_json_entry(const DatabaseEntity entity)
    {
        json result_json = {{"id", entity.id()}, {"alias", entity.alias()}};
        return result_json;
    }

    template <typename T>
    std::enable_if_t<std::is_same<T, use_name_tag>::value, json>
    build_json_entry(const DatabaseEntity entity)
    {
        json result_json = {{"id", entity.id()}, {"name", entity.name()}};
        return result_json;
    }

    template <typename T>
    std::enable_if_t<std::is_same<T, use_username_tag>::value, json>
    build_json_entry(const DatabaseEntity entity)
    {
        json result_json = {{"id", entity.id()}, {"username", entity.username()}};
        return result_json;
    }

  public:
    /**
     * Returns a JSON array with the result from the search.
     *
     * The JSON looks like this:
     *     [ {id: ${ENTITY_ID}},
     *       {name|alias|username: ${ENTITY_NAME_OR_ALIAS}}
     *     ]
     */
    json search_json(DBPtr db, const std::string &partial)
    {
        auto entities = search(db, partial);
        json rep;
        for (const auto &entity : entities)
        {
            rep.push_back(build_json_entry<AliasOrName>(entity));
        }
        return rep;
    }
};
}
}
}
