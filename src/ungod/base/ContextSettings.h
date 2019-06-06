/*
* This file is part of the ungod - framework.
* Copyright (C) 2016 Felix Becker - fb132550@uni-greifswald.de
*
* This software is provided 'as-is', without any express or
* implied warranty. In no event will the authors be held
* liable for any damages arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute
* it freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented;
*    you must not claim that you wrote the original software.
*    If you use this software in a product, an acknowledgment
*    in the product documentation would be appreciated but
*    is not required.
*
* 2. Altered source versions must be plainly marked as such,
*    and must not be misrepresented as being the original software.
*
* 3. This notice may not be removed or altered from any
*    source distribution.
*/

#ifndef CONTEXT_SETTINGS_H
#define CONTEXT_SETTINGS_H

#include <memory>

namespace ungod
{
    /**
    * \brief An object that contains handles to parts of the engine and is given around as
    * argument to methods. Depending on the application-context
    * there can numberous objects that are usefull to access overall through the program.
    * They must be bundled in a structure and then be embedded via a embed<STRUCT> call. If you want to
    * access these properties in a context, where you know your struct is embedded, you can
    * get them via a access<STRUCT> call. Note that accessing a type that was not embedded in the
    * ContextSettings objects results in undefined behavior.
    */
    struct ContextSettings
    {
        //constructor
        ContextSettings() {}
        ContextSettings(const ContextSettings&) = delete;
        ContextSettings& operator=(const ContextSettings&) = delete;
        ContextSettings(ContextSettings &&) = delete;
        ContextSettings& operator=(ContextSettings &&) = delete;

        template<typename STRUCT>
        void embed(const STRUCT& s);

        template<typename STRUCT>
        const STRUCT& access() const;

        template<typename STRUCT>
        STRUCT& modify();

        virtual ~ContextSettings() {}

    private:
        struct ContextParam
        {
        };

        template<typename STRUCT>
        struct DefinitiveParam : public ContextParam
        {
            STRUCT param;
            DefinitiveParam(const STRUCT& s) : param(s) {}
        };

        std::unique_ptr<ContextParam> context;
    };


    template<typename STRUCT>
    void ContextSettings::embed(const STRUCT& s)
    {
        context = std::unique_ptr<ContextParam>(new DefinitiveParam<STRUCT>(s));
    }

    template<typename STRUCT>
    const STRUCT& ContextSettings::access() const
    {
        return static_cast<DefinitiveParam<STRUCT>*>(context.get())->param;
    }

    template<typename STRUCT>
    STRUCT& ContextSettings::modify()
    {
        return static_cast<DefinitiveParam<STRUCT>*>(context.get())->param;
    }
}

#endif // CONTEXT_SETTINGS_H

