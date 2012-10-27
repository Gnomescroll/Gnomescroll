#pragma once

// No names can be reused ever
// 2 files: active names and dead names

class DatNameMap
{    
    public:
        const size_t max;
        const size_t name_max;
        
        size_t size;
        char* originals;
        char* replacements;

    bool add_definition(const char* original, const char* replacement)
    {
        if (this->size >= this->max) return false;
        if (strcmp(original, replacement) == 0) return false;

        size_t index = this->get_index(this->size);
        strncpy(&this->originals[index], original, this->name_max);
        this->originals[index + this->name_max] = '\0';
        strncpy(&this->replacements[index], replacement, this->name_max);
        this->replacements[index + this->name_max] = '\0';
        this->size++;
        
        return true;
    }

    const char* get_mapped_name(const char* original)
    {
        for (size_t i=0; i<this->size; i++)
        {
            size_t index = this->get_index(i);
            if (strcmp(&this->originals[index], original) == 0)
                return &this->replacements[index];
        }
        return NULL;
    }

    void condense()
    {
        // condense transitive relationships
        // e.g. if A->B and B->C, then change to A->C
        // repeatedly apply until the mapping is stable
        // i.e. no string in replacements is also in originals
        bool condensed = true;
        do
        {
            condensed = true;
            for (size_t i=0; i<this->size; i++)
            {
                size_t index = this->get_index(i);
                const char* replacement = &this->replacements[index];
                const char* dest = this->get_mapped_name(replacement);
                if (dest == NULL) continue;

                // make sure name does not resolve to self, for circular definitions
                bool same = (strcmp(&this->originals[index], dest) == 0);
                if (same)
                    printf("ERROR: chained name mapping resolves to self, for name %s\n", dest); 
                GS_ASSERT_ABORT(!same);
                
                strncpy(&this->replacements[index], dest, this->name_max+1);
                this->replacements[index+this->name_max] = '\0';
                condensed = false;
            }
        }
        while (!condensed);
    }

    DatNameMap(const size_t max, const size_t name_max) :
        max(max), name_max(name_max), size(0)
    {
        this->originals = (char*)calloc(this->max * (this->name_max+1), sizeof(char));
        this->replacements = (char*)calloc(this->max * (this->name_max+1), sizeof(char));
    }

    private:

    size_t get_index(size_t index)
    {
        return index * (this->name_max+1);
    }
};
