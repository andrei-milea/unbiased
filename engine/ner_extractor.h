#ifndef _NER_EXTRACTOR_H
#define _NER_EXTRACTOR_H

#include <string>
#include <unordered_map>
#include <vector>

namespace ner
{

enum class EntityType
{
    PERSON,
    ORG,
    PLACE
}

struct EntityInfo
{
    std::vector<std::string> word_before_vec;
    std::vector<std::string> word_after_vec;
}

class NERExtractor
{
public:
    NERExtractor(const std::unordered_map<std::string, EntityInfo>& entities_map)
        : entities_map_(entities_map)
    {
    }

    std::vector<std::string> find_entities(const std::string& text) const;

private:
    std::unordered_map<std::string, EntityInfo> entities_map_;
};

}

#endif
