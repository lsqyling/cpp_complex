#include "glob.h"

struct mysinglton
{
};



int main(int argc, char **argv)
{
    single::glob<mysinglton> glob;
    glob.new_();
    mysinglton *ptr = glob.get();
    glob.delete_();

    return 0;
}