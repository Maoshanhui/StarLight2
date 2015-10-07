//
//  Ninja.h
//  MyCppGame
//
//  Created by 毛善辉 on 15/10/4.
//
//

#ifndef Ninja_h
#define Ninja_h
#include "cocos2d.h"

class Ninja :
public cocos2d::Node
{
public:
    Ninja();
    ~Ninja();
    
public:
    
    CREATE_FUNC( Ninja );
    
    static Ninja* create( int RoleId );
    virtual bool initMySprite( int RoleId );
    
public:
    
    //void CreateActions( int RoleId  );
    
    enum class State
    {
        None ,
        
        WalkLeft ,
        
        WalkRight ,
        
    };
    void SetState( State type );
    
    const Ninja::State getState();
    
    void manageMonster( );
    
    void update( float xL , float xR );
    
private:
    
    State m_State;
    
    int m_RoleId;
    
    cocos2d::Size m_size;
    
    cocos2d::Sprite* m_pSprMain;
    
    float m_xL;
    float m_xR;
};


#endif /* Ninja_h */
