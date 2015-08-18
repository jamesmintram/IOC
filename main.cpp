//
//  main.cpp
//  IOC
//
//  Created by James Mintram on 18/08/2015.
//  Copyright © 2015 James Mintram. All rights reserved.
//

#include <iostream>
#include <map>
#include <assert.h>

#define DEFINE_INJECTABLE_SERVICE(ServiceType)\
    inline const char* GetServiceName(ServiceType* dontUse) {return #ServiceType;}

#define DEFINE_INJECTABLE_SERVICE_CLASS(ServiceClass, ServiceType)\
    inline const char* GetServiceName(ServiceClass* dontUse) {return GetServiceName((ServiceType*)0);}


class ServiceRegistry
{
public:
    template<typename ServiceType>
    void RegisterService(ServiceType* service)
    {
        std::string serviceName = GetServiceName((ServiceType*)0);
        //Service already registered
        assert(services.find(serviceName) == services.end());
        services[serviceName] = service;
    }
    template<typename ServiceType>
    ServiceType* GetService()
    {
        std::string serviceName = GetServiceName((ServiceType*)0);
        //Service not registered
        assert(services.find(serviceName) != services.end());
        
        return (ServiceType*)services[GetServiceName((ServiceType*)0)];
    }
protected:
    std::map<std::string, void*> services;
};

template<typename ServiceType>
class Injectable
{
public:
    Injectable(ServiceRegistry& registry)
    {
        service = registry.GetService<ServiceType>();
        printf("Service name: %s\n", GetServiceName((ServiceType*)0));
    }
    ServiceType* Get() {return service;}
    
    ServiceType* operator-> ()
    {
        return service;
    }
protected:
    ServiceType* service;
};



class IAchievementsService
{
public:
    virtual int GetVal() = 0;
};
class AchievementsService
{
public:
    AchievementsService(int _val) : val(_val){}
    virtual int GetVal() {return val;}
protected:
    int val;
};
class FacebookService
{
};

//  Need to be visible wherever Injectables are used
DEFINE_INJECTABLE_SERVICE(IAchievementsService)

//  Only need to be visible where the injectables are
//  registered with a Registry
DEFINE_INJECTABLE_SERVICE_CLASS(AchievementsService, IAchievementsService)
DEFINE_INJECTABLE_SERVICE(FacebookService)

class MyGameObject
{
public:
    MyGameObject(ServiceRegistry& registry)
        : Achievements(registry)
        , Facebook(registry)
    {}
    Injectable<AchievementsService> Achievements;
    Injectable<FacebookService> Facebook;
};

int main(int argc, const char * argv[]) {
    
    ServiceRegistry registry;
    registry.RegisterService(new AchievementsService(12));
    registry.RegisterService(new FacebookService());
    
    MyGameObject myObject(registry);

    printf("Value: %d", myObject.Achievements->GetVal());
    return 0;
}
