// Copyright 2009-2018 NTESS. Under the terms
// of Contract DE-NA0003525 with NTESS, the U.S.
// Government retains certain rights in this software.
//
// Copyright (c) 2009-2018, NTESS
// All rights reserved.
//
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.


#ifndef SST_CORE_MODEL_ELEMENT_PYTHON_H
#define SST_CORE_MODEL_ELEMENT_PYTHON_H

#include <vector>
#include <string>


namespace SST {

typedef void* (*genPythonModuleFunction)(void);


/** Class to represent the code that needs to be added to create the
 * python module struture for the library.
 */
class SSTElementPythonModuleCode {

private:
    //! Pointer to parent module.
    /** Parent module.  This will be NULL if this is the primary module (i.e. sst.*) */
    SSTElementPythonModuleCode* parent;

    //! Simple name of the module.
    /** Simple name of the module.  Full name will be parant_full_name.module_name */
    std::string module_name;

    //! Code to be compiled
    ///
    /// The code to be added under the specified module name.  Python
    /// files will need to be turned into a char array.  One way to do
    /// this is to use the following in your Makefile:
    /// %.inc: %.py
    ///         od -v -t x1 < $< | sed -e 's/^[^ ]*[ ]*//g' -e '/^\s*$$/d' -e 's/\([0-9a-f]*\)[ $$]*/0x\1,/g' > $@
    ///
    // (Used different comment style because command above would have ended a c-style comment block)
    char* code;

    //! Filename used when reporting errors
    /** Filename that will be used in any error messages generated
     * when loading the python module into the interpreter.
     */
    std::string filename;

    //! Private constructor.  Called by friend class SSTElementPythonModule.
    /**
     *  Private constructor.  Called by friend class SSTElementPythonModule.
     *  \param parent pointer to parent module
     *  \param module_name simple name of the module
     *  \param code code to be compiled
     *  \param filename filname used when reporting errors
     *  \sa parent, module_name, code, filename
     */
    SSTElementPythonModuleCode(SSTElementPythonModuleCode* parent, const std::string& module_name, char* code = NULL, std::string filename = "") :
        parent(parent),
        module_name(module_name),
        code(code),
        filename(filename)
    {
        // If no filename for debug is given, just set it to the full name of the module
        if ( filename == "" ) {
            filename = getFullModuleName();
        }
    }


    //! load the code as a module into the interpreter
    /**
     *  Load the code as a module into the interpreter.
     *  Both return type and pyobj are actually PyObject*, but passing
     *  as a void* to avoid including python header files in base SST
     *  header files.
     *  \param parent_module parent module (passed as PyObject*)to load this module under
     *  \return pointer (as PyObject*) to the created module
     */
    void* load(void* parent_module);
 
    //! Vector of sub_modules
    std::vector<SSTElementPythonModuleCode*> sub_modules;


    friend class SSTElementPythonModule;
    
public:

    //! Add a submodule to the module
    ///
    /// Python files will need to be turned into a char array (code parameter).  One way to do
    /// this is to use the following in your Makefile:
    /// \verbatim %.inc: %.py 
    ///         od -v -t x1 < $< | sed -e 's/^[^ ]*[ ]*//g' -e '/^\s*$$/d' -e 's/\([0-9a-f]*\)[ $$]*/0x\1,/g' > $@ \endverbatim
    /// \param module_name simple name of the module
    /// \param code code to be compiled
    /// \param filename filname used when reporting errors
    ///
    /// 
    SSTElementPythonModuleCode* addSubModule(const std::string& module_name, char* code = NULL, std::string filename = "");

    //! Get the full name of the module
    /**
     *  Get the full name of the module formatted as parent_full_name.module_name.
     *  \return full name of module as a string
     */
    std::string getFullModuleName();

};


/**
   Base class for python modules in element libraries.

   Element libraries can include a class derived from this class and
   create a python module hierarchy.
 */
class SSTElementPythonModule {

protected:
    std::string library;
    std::string pylibrary;
    std::string sstlibrary;
    char* primary_module;

    std::vector<std::pair<std::string,char*> > sub_modules;

    SSTElementPythonModuleCode* primary_code_module;
    
    // Only needed for supporting the old ELI
    SSTElementPythonModule() {}
    
public:
    virtual ~SSTElementPythonModule() {}
    
    //! Constructor for SSTElementPythonModule.  Must be called by derived class
    /**
     * \param library name of the element library the module is part
     * of.  Primary module name will be sst.library and submodules
     * under this can also be created.
     */ 
    SSTElementPythonModule(std::string library);

    
    __attribute__ ((deprecated("Support for addPrimaryModule will be removed in version 9.0.  Please use createPrimaryModule().")))
    void addPrimaryModule(char* file);

    __attribute__ ((deprecated("Support for addPrimaryModule will be removed in version 9.0.  Please use createPrimaryModule() to get an SSTElementPythonModuleCode object then use it's addSubModule() method.")))
    void addSubModule(std::string name, char* file);

    virtual void* load();

    //! Create the top level python module (i.e. the one named sst.library)
    /// Python files will need to be turned into a char array (code parameter).  One way to do
    /// this is to use the following in your Makefile:
    /// \verbatim %.inc: %.py 
    ///         od -v -t x1 < $< | sed -e 's/^[^ ]*[ ]*//g' -e '/^\s*$$/d' -e 's/\([0-9a-f]*\)[ $$]*/0x\1,/g' > $@ \endverbatim
    /// \param code code to be compiled
    /// \param filename filname used when reporting errors
    ///
    ///     
    SSTElementPythonModuleCode* createPrimaryModule(char* code = NULL, std::string filename = "");

};

// Class to use to support old ELI
class SSTElementPythonModuleOldELI : public SSTElementPythonModule {
private:
    genPythonModuleFunction func;
    
public:
    SSTElementPythonModuleOldELI(genPythonModuleFunction func) :
        SSTElementPythonModule(),
        func(func)
        {
        }

    void* load() override {
        return (*func)();
    }
};

}

#endif // SST_CORE_MODEL_ELEMENT_PYTHON_H
