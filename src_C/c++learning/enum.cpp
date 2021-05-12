#include <iostream>
using namespace std;
enum ss{
    hello,
    world
};
enum class st{
    dodo,
    hello,
    world
};
#define FORALL_NS_SYMBOLS(_)         \
  _(namespaces, attr)                \
  _(attr, transB)                    \
  _(attr, name)
  
enum class _keys : unique_t {
    #define DEFINE_KEY(ns, s) ns##_##s,
    FORALL_NS_SYMBOLS(DEFINE_KEY)
    #undef DEFINE_KEY
    num_symbols
};

#define DEFINE_SYMBOL(s) \
  constexpr Symbol s(static_cast<unique_t>(_keys::s));

#undef DEFINE_SYMBOL

#define DEFINE_SYMBOL(ns, s) \
  namespace ns { constexpr Symbol s(static_cast<unique_t>(_keys::ns##_##s)); }
FORALL_NS_SYMBOLS(DEFINE_SYMBOL)
#undef DEFINE_SYMBOL
  
int main(int argc, char * argv[]){
    cout<<"hello:"<<hello<<"\n";
    cout<<"world:"<<world<<"\n";
    st s = st::hello;
    using st_rep_type = std::underlying_type<st>::type;

    cout<<"hello:"<<static_cast<st_rep_type>(s)<<"\n";
    // cout<<"world:"<<st::world<<"\n";
}