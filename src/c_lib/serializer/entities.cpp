#include "entities.hpp"

#include <entity/objects.hpp>
#include <serializer/logger.hpp>

namespace serializer
{

static bool parse_entity_file_header_token(const char* key, const char* val, class ParsedEntityFileData* data)
{
    bool err = false;
    if (strcmp(VERSION_TAG, key) == 0)
    {
        long long version = parse_int(val, err);
        IF_ASSERT(err || version <= 0 || version > GS_VERSION) return false;
        data->version = version;
    }
    else
    if (strcmp(ENTITY_COUNT_TAG, key) == 0)
    {
        long long entity_count = parse_int(val, err);
        IF_ASSERT(err || entity_count < 0 || entity_count > INT_MAX) return false;
        data->entity_count = entity_count;
    }
    else
    {
        GS_ASSERT(false);
        return false;
    }
    return true;
}

static bool parse_entity_token(const char* key, const char* val, class ParsedEntityData* data)
{
    bool err = false;
    if (strcmp(key, NAME_TAG) == 0)
    {
        bool valid_name = Entities::is_valid_entity_name(val);
        IF_ASSERT(!valid_name) return false;
        strncpy(data->name, val, DAT_NAME_MAX_LENGTH);
        data->name[DAT_NAME_MAX_LENGTH] = '\0';
    }
    else
    if (strcmp(key, ENTITY_ID_TAG) == 0)
    {
        long long entity_id = parse_int(val, err);
        IF_ASSERT(err || entity_id <= 0) return false;
        data->entity_id = entity_id;
    }
    else
    {
        GS_ASSERT(false);
        return false;
    }
    return true;
}

static bool parse_energy_core_token(const char* key, const char* val, class ParsedEnergyCoreData* data)
{
    IF_ASSERT(strcmp(key, MAP_POSITION_TAG) != 0)
        return false;
    else if (!parse_map_position(val, &data->position))
        return false;
    return true;
}

static bool parse_agent_spawner_token(const char* key, const char* val, class ParsedAgentSpawnerData* data)
{
    bool err = false;
    if (strcmp(key, MAP_POSITION_TAG) == 0)
    {
        if (!parse_map_position(val, &data->position))
            return false;
    }
    else
    if (strcmp(key, USER_COUNT_TAG) == 0)
    {
        long long user_count = parse_int(val, err);
        IF_ASSERT(err || user_count < 0 || (size_t)user_count > Entities::MAX_USERS_PER_AGENT_SPAWNER)
            return false;
        data->set_user_count(user_count);
    }
    else
    if (strcmp(key, USER_ID_TAG) == 0)
    {
        long long user_id = parse_int(val, err);
        IF_ASSERT(err) return false;
        IF_ASSERT(!isValid((UserID)user_id)) return false;
        if (!data->add_user((UserID)user_id)) return false;
    }
    else
    {
        IF_ASSERT(true) return false;
    }
    return (!err);
}

bool process_entities_blob(const char* str, size_t filesize)
{
    const size_t LONGEST_LINE = 0xFF;
    static char buf[LONGEST_LINE+1] = {'\0'};

    // copy file header line
    size_t i = 0;
    char c = '\0';
    while ((c = str[i]) != '\0' && c != '\n' && i < LONGEST_LINE)
        buf[i++] = c;
    buf[i] = '\0';
    IF_ASSERT(c != '\n') return false;

    // read file header
    class ParsedEntityFileData entity_file_data;
    parse_line<class ParsedEntityFileData>(&parse_entity_file_header_token, buf, i, &entity_file_data);
    IF_ASSERT(!entity_file_data.valid) return false;
    i++;

    // process entities
    class ParsedEntityData entity_data;
    class ParsedEnergyCoreData energy_core_data;
    class ParsedAgentSpawnerData agent_spawner_data;
    for (int m=0; m<entity_file_data.entity_count; m++)
    {
        IF_ASSERT(c == '\0') return false;

        // copy header to buffer
        size_t k = 0;
        while ((c = str[i++]) != '\0' && c != '\n' && k < LONGEST_LINE)
            buf[k++] = c;
        buf[k] = '\0';
        IF_ASSERT(c != '\n') return false;

        // parse entity header
        entity_data.reset();
        parse_line<class ParsedEntityData>(&parse_entity_token, buf, k, &entity_data);
        IF_ASSERT(!entity_data.valid) return false;
        GS_ASSERT(entity_data.entity_id == m+1);    // not fatal, but indicates problem

        // get mapped name
        const char* actual_name = Entities::get_compatible_entity_name(entity_data.name);
        IF_ASSERT(actual_name == NULL) return false;

        // get type
        EntityType entity_type = Entities::get_entity_type(actual_name);
        IF_ASSERT(entity_type == NULL_ENTITY_TYPE) return false;

        // create entity
        class Entities::Entity* entity = Entities::create(entity_type);
        IF_ASSERT(entity == NULL) return false;

        bool err = false;
        struct Vec3i* position = NULL;
        if (entity_type == ENTITY_ENERGY_CORE)
        {
            position = &energy_core_data.position;
            i += parse_block<class ParsedEnergyCoreData>(&parse_energy_core_token, ENTITY_SEPARATOR, &str[i], &energy_core_data);
            GS_ASSERT(energy_core_data.valid);
            err = (!energy_core_data.valid);
        }
        else if (entity_type == ENTITY_AGENT_SPAWNER)
        {
            position = &agent_spawner_data.position;
            i += parse_block<class ParsedAgentSpawnerData>(&parse_agent_spawner_token, ENTITY_SEPARATOR, &str[i], &agent_spawner_data);
            GS_ASSERT(agent_spawner_data.valid);
            err = (!agent_spawner_data.valid);
            if (agent_spawner_data.user_count == agent_spawner_data.added_users)
            {
                auto spawner = GET_COMPONENT(AgentSpawner, entity);
                GS_ASSERT(spawner != NULL);
                err = (spawner == NULL);
                if (spawner != NULL)
                    for (size_t i=0; i<agent_spawner_data.user_count; i++)
                        spawner->add_user(agent_spawner_data.users[i]);
            }
            else err = true;
        }
        else
        {
            err = true;
            GS_ASSERT(false);
        }

            auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
        GS_ASSERT(physics != NULL);
        err = (physics == NULL);
        if (position != NULL && physics != NULL)
            physics->set_position(vec3_init(position->x, position->y, position->z));

        IF_ASSERT(err)
        {   // cancel entity
            Entities::destroy(entity);
            return false;
        }
        else
        {   // ready entity
            Entities::ready(entity);
        }
    }

    return true;
}

bool load_entities_file(const char* fn)
{
    printf("Loading entity file %s\n", fn);

    size_t size = 0;
    char* buf = read_file_to_buffer(fn, &size);
    IF_ASSERT(buf == NULL) return false;

    bool success = process_entities_blob(buf, size);
    free(buf);
    IF_ASSERT(!success)
    {
        log_entity_load_error("Error processing entire file");
        Entities::destroy_all();
        return false;
    }

    return true;
}

static bool write_entity_header(FILE* f, class Entities::Entity* entity, int id)
{
    const char* name = Entities::get_entity_name(entity->type);
    IF_ASSERT(name == NULL) return false;

    // write name, id
    int ret = fprintf(f, ENTITY_FMT, name, id);
    IF_ASSERT(ret <= 0)
    {
        log_entity_save_error("Failed to write entity to file", name, id);
        return false;
    }
    else
    {
        ret = fprintf(f, "\n");
        GS_ASSERT(ret > 0);
    }
    return true;
}

static bool write_position_tag(FILE* f, struct Vec3 position)
{
    int ret = fprintf(f, MAP_POSITION_FMT, (int)position.x, (int)position.y, (int)position.z);
    IF_ASSERT(ret <= 0) return false;
    ret = fprintf(f, "\n");
    GS_ASSERT(ret > 0);
    return (ret > 0);
}

static bool write_entity_terminator(FILE* f)
{
    int ret = fprintf(f, ENTITY_SEPARATOR "\n");
    GS_ASSERT(ret > 0);
    return (ret > 0);
}

static bool write_user_id_tag(FILE* f, UserID user_id)
{
    IF_ASSERT(!isValid(user_id)) return false;
    int ret = fprintf(f, USER_ID_FMT, user_id);
    IF_ASSERT(ret <= 0) return false;
    ret = fprintf(f, "\n");
    GS_ASSERT(ret > 0);
    return (ret > 0);
}

static bool write_user_count_tag(FILE* f, size_t user_count)
{
    int ret = fprintf(f, USER_COUNT_FMT, (long long unsigned)user_count);
    IF_ASSERT(ret <= 0) return false;
    ret = fprintf(f, "\n");
    GS_ASSERT(ret > 0);
    return (ret > 0);
}

static bool save_agent_spawner(FILE* f, class Entities::Entity* entity, int id)
{
    // unpack all component data we need
    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
    IF_ASSERT(physics == NULL) return false;

    auto spawner = GET_COMPONENT(AgentSpawner, entity);
    IF_ASSERT(spawner == NULL) return false;

    if (!write_entity_header(f, entity, id)) return false;
    if (!write_position_tag(f, physics->get_position())) return false;

    if (!write_user_count_tag(f, spawner->users.count)) return false;

    for (size_t i=0; i<spawner->users.count; i++)
        if (!write_user_id_tag(f, spawner->users.subscribers[i]))
            return false;

    if (!write_entity_terminator(f)) return false;

    return true;
}


static bool save_energy_core(FILE* f, class Entities::Entity* entity, int id)
{
    // unpack all component data we need
    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
    IF_ASSERT(physics == NULL) return false;

    if (!write_entity_header(f, entity, id)) return false;
    if (!write_position_tag(f, physics->get_position())) return false;

    if (!write_entity_terminator(f)) return false;

    return true;
}

static bool write_entity_file_header(FILE* f, int entity_ct)
{
    int ret = fprintf(f, ENTITY_FILE_HEADER_FMT, GS_VERSION, entity_ct);
    IF_ASSERT(ret <= 0)
    {
        log_entity_save_error("Failed to write entity file header");
        return false;
    }
    else
    {
        ret = fwrite("\n", sizeof(char), 1, f);
        GS_ASSERT(ret > 0);
    }
    return true;
}

bool save_entities()
{
    bool err = false;
    int ct = 0;

    int agent_spawner_ct = 0;
    int energy_core_ct = 0;

    // collect the entities
    EntityType agent_spawner_type = Entities::get_entity_type("agent_spawner");
    IF_ASSERT(agent_spawner_type == NULL_ENTITY_TYPE) return false;

    char* used_agent_spawners = NULL;
    int agent_spawners_max = 0;
    Entities::Entity** agent_spawners = Entities::get_all(agent_spawner_type,
                                                          used_agent_spawners,
                                                          agent_spawners_max);
    IF_ASSERT(agent_spawners == NULL || used_agent_spawners == NULL) return false;

    EntityType energy_core_type = Entities::get_entity_type("energy_core");
    IF_ASSERT(energy_core_type == NULL_ENTITY_TYPE) return false;

    char* used_energy_cores = NULL;
    int energy_cores_max = 0;
    Entities::Entity** energy_cores = Entities::get_all(energy_core_type,
                                                        used_energy_cores,
                                                        energy_cores_max);
    IF_ASSERT(energy_cores == NULL || used_energy_cores == NULL) return false;

    FILE* f = fopen(entity_path_tmp, "w");
    IF_ASSERT(f == NULL) return false;

    // write a temporary file header.   the actual count will be to be written later
    err = (!write_entity_file_header(f, agent_spawners_max + energy_cores_max));
    IF_ASSERT(err) goto Error;

    // save the entities
    for (int i=0; i<agent_spawners_max; i++)
        if (used_agent_spawners[i])
        {
            if (save_agent_spawner(f, agent_spawners[i], i+ct+1))
                agent_spawner_ct++;
            else
            {
                GS_ASSERT(false);
            }
        }

    ct += agent_spawner_ct;

    for (int i=0; i<energy_cores_max; i++)
        if (used_energy_cores[i])
        {
            if (save_energy_core(f, energy_cores[i], i+ct+1))
                energy_core_ct++;
            else
            {
                GS_ASSERT(false);
            }
        }

    ct += energy_core_ct;

    // rewrite the header with the actual amounts
    rewind(f);
    err = (!write_entity_file_header(f, ct));
    IF_ASSERT(err) goto Error;

    Error:  // GOTO LABEL
        int ferr = fclose(f);
        IF_ASSERT(ferr)
            log_entity_save_error("Failed to close entity save file");

    return (!err && !ferr);
}

bool load_entities()
{
    if (file_exists(entity_path) && get_filesize(entity_path) > 0)
        return load_entities_file(entity_path);
    else
    if (file_exists(entity_path_bak) && get_filesize(entity_path_bak) > 0)
        return load_entities_file(entity_path_bak);
    else
        printf("WARNING: No entities file found\n");
    return true;
}

}   // serializer

