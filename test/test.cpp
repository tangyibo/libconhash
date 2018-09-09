#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "conhash.h"
#include <string>
#include <map>
#include <iostream>

class Conhash
{
public:

    Conhash() 
    {
        _conhash = conhash_init(NULL);
        assert(NULL!=_conhash);
        
        _nodemaps.clear();
    }

    ~Conhash() 
    {
        std::map<std::string, void*>::iterator it;
        for (it = _nodemaps.begin(); it != _nodemaps.end(); ++it) 
        {
            struct node_s *tmp = (struct node_s *) (it->second);
            assert(NULL!=tmp);
            delete tmp;
        }
        
        _nodemaps.clear();
        
        if (NULL != _conhash) 
        {
            conhash_fini(_conhash);
            _conhash = NULL;
        } 
    }
    
    void add_node(const char *iden,const int replica)
    {
        struct node_s *node=new(std::nothrow) struct node_s;
        assert(NULL!=node);
        
        conhash_set_node(node, iden,replica);
        conhash_add_node(_conhash, node);
        _nodemaps[iden] = node;
    }
    
    void delete_node(const char *iden)
    {
        std::map<std::string,void*>::iterator it;
        it=_nodemaps.find(iden);
        if(_nodemaps.end()!=it)
        {
            struct node_s *node=(struct node_s *)(it->second);
            conhash_del_node(_conhash, node);
            delete node;
            node=NULL;
            
            _nodemaps.erase(it);
        }
    }
    
    const char* lookup_node(const char *keystr)
    {
        const struct node_s *node = conhash_lookup(_conhash, keystr);
        if (NULL != node) 
        {
            std::map<std::string, void*>::iterator it;
            for (it = _nodemaps.begin(); it != _nodemaps.end(); ++it) 
            {
                struct node_s *tmp = (struct node_s *) (it->second);
                if (tmp == node)
                    return it->first.c_str();
            }
        }
        
        return NULL;
    }
    
    
    inline const int vnodes_num()
    {
        return conhash_get_vnodes_num(_conhash);
    }
    
    inline const int rnodes_num()
    {
        return (int)_nodemaps.size();
    }

protected:
    Conhash(const Conhash&);
    Conhash& operator=(const Conhash&);
    
private:
    struct conhash_s * _conhash;
    std::map<std::string,void*> _nodemaps;
};

int main(int argc,char *argv[])
{
    Conhash hash;
    hash.add_node("192.168.100.1;10091", 50);
    hash.add_node("192.168.100.2;10091", 50);
    hash.add_node("192.168.100.3;10091", 50);
    hash.add_node("192.168.100.4;10091", 50);

    std::cout << "virtual nodes number :" << hash.vnodes_num() << std::endl;
    std::cout << "real nodes number :" << hash.rnodes_num() << std::endl;
    std::cout << "the hashing results------------------:" << std::endl;

    for (int i = 1; i <= 200; i++) 
    {
        char str[64];
        sprintf(str, "Redis-key.km%03d", i);
        const char *rnode=hash.lookup_node(str);
        std::cout<<"["<<str<<"] is in node: ["<<rnode<<"]"<<std::endl;
        
        if(i==15)
        {
            hash.delete_node("192.168.100.4;10091");
        }
    }
    
    return 0;
}