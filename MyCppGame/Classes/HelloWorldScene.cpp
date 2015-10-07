#include "HelloWorldScene.h"
#include "CCShake.h"
USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    // physics scene
    auto scene = Scene::createWithPhysics();
    
    // physics debug
    //scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();
    
    scene->getPhysicsWorld()->setGravity(Vect(0.0f, -298.0f));
    
    scene->getPhysicsWorld()->setAutoStep(false);
    
    layer->setPhyWorld(scene->getPhysicsWorld());
    
    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    log(" hello init");
    
    visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    // background node
    bgNode = Node::create();
    bgNode->setContentSize( Size( 1600 , 1120 ) );
    bgNode->setAnchorPoint(Vec2::ZERO);
    bgNode->setPosition( Vec2::ZERO );
    
    // all other node
    bulletNode = Node::create();
    brickNode = Node::create();
    manageNode = Node::create();
    coinNode = Node::create();
    solidNode = Node::create();
    rockNode = Node::create();
    
    setTouchEnabled(true);
     //one point touch
    setTouchMode(Touch::DispatchMode::ALL_AT_ONCE);
    

    // TiledMap
    tiledMap = TMXTiledMap::create("map.tmx");
    mapWidth = tiledMap->getMapSize().width;
    mapHeight = tiledMap->getMapSize().height;
    //log("w =  %d , h = %d", mapWidth , mapHeight );
    
    // all layers
    backgroudLayer = tiledMap->getLayer( "background" );
    bornPoints = tiledMap->getObjectGroup( "born" );
    manageLayer = tiledMap->getLayer( "manage" );
    coinLayer = tiledMap->getLayer( "coin" );
    solidLayer = tiledMap->getLayer( "solid" );
    rockLayer = tiledMap->getLayer( "rock" );
    
    // load layers to node
    for(int i = 0 ; i < mapWidth; ++i )
        for(int j = 0 ; j < mapHeight; ++j )
        {
            if (backgroudLayer->getTileGIDAt(Vec2( i , j )) != 0)
            {
                auto body = PhysicsBody::createBox( Size( 32.0 ,32.0 ),PhysicsMaterial(2.0f,0.5f,0.0f) );
                body->setDynamic(false);
                
                auto boxnode = Node::create();
                boxnode->setContentSize( Size(32.0,32.0));
                boxnode->setPosition( tiledToVec( i , j ) );
                
                boxnode->setPhysicsBody( body );
                brickNode->addChild( boxnode );
                
            }
            if (manageLayer->getTileGIDAt(Vec2( i , j )) != 0)
            {
                auto managenode = Node::create();
                managenode->setContentSize( Size(32.0 , 32.0 ) );
                managenode->setPosition( tiledToVec( i , j) );
                manageNode->addChild( managenode );
            }
            if (coinLayer->getTileGIDAt(Vec2( i , j )) != 0)
            {
                auto coinnode = Node::create();
                coinnode->setContentSize( Size(32.0 , 32.0 ) );
                coinnode->setPosition( tiledToVec( i , j) );
                
                Point *p = new Point( i , j );
                coinnode->setUserData( p );
                
                coinNode->addChild( coinnode );
            }
            if (solidLayer->getTileGIDAt(Vec2( i , j )) != 0)
            {
                auto body = PhysicsBody::createBox( Size( 32.0 ,32.0 ),PhysicsMaterial(2.0f,0.5f,0.0f) );
                body->setDynamic(false);
                
                auto solidnode = Node::create();
                solidnode->setContentSize( Size(32.0 , 32.0 ) );
                solidnode->setPosition( tiledToVec( i , j) );
                
                Point *p = new Point( i , j );
                solidnode->setUserData( p );
                
                solidnode->setPhysicsBody( body );
                solidNode->addChild( solidnode );
            }
            if (rockLayer->getTileGIDAt(Vec2( i , j )) != 0)
            {
                auto body = PhysicsBody::createBox( Size( 32.0 ,32.0 ),PhysicsMaterial(2.0f,0.5f,0.0f) );
                body->setDynamic(false);
                
                auto rocknode = Node::create();
                rocknode->setContentSize( Size(32.0 , 32.0 ) );
                rocknode->setPosition( tiledToVec( i , j) );
                
                Point *p = new Point( i , j );
                rocknode->setUserData( p );
                
                rocknode->setPhysicsBody( body );
                rockNode->addChild( rocknode );
            }
            
        }
    
    tiledMap->setPosition( Vec2::ZERO );
    bgNode->addChild(tiledMap);
    
    
    // define world edge
    auto body = PhysicsBody::createEdgeBox(tiledMap->getContentSize(),PHYSICSBODY_MATERIAL_DEFAULT,5.0f);
    edgeNode = Node::create();
    edgeNode->setPosition( tiledMap->getContentSize().width * 0.5 ,tiledMap->getContentSize().height * 0.5);
    edgeNode->setPhysicsBody(body);
    bgNode->addChild(edgeNode);
    
    // Player
    ValueMap playerBorn = bornPoints->getObject( "playerBorn" );
    float x = playerBorn.at( "x" ).asFloat() ;
    float y = playerBorn.at( "y" ).asFloat() ;
    
    //log("x = %f , y = %f" , x, y );
    
    player = Ninja::create( 0 );
    player->setPosition( Vec2(x,y) );
    
    playerBody = PhysicsBody::createBox( Size(35.0,48.0) ,PhysicsMaterial(0.0f,0.1f,0.2f));
    playerBody->setMass( 20.0 );
    playerBody->setRotationEnable(false);
    playerBody->setDynamic( true );
    player->setPhysicsBody( playerBody );
    
    bgNode->addChild( player );
    
    // monster
    ValueMap monsterBorn;
    for(int i = 0 ; i < 12 ; ++i )
    {
        monsterBorn = bornPoints->getObject( "monsterBorn"+std::to_string( i ) );
        x = monsterBorn.at( "x" ).asFloat() ;
        y = monsterBorn.at( "y" ).asFloat() ;
        monsters[ i ] =  Ninja::create( 1 );
        monsters[ i ]->setPosition( Vec2( x, y ) );
        monsters[ i ]->setVisible( false );
        bgNode->addChild( monsters[ i ]);
    }
    
    //Camera
    //m_Camera = Camera::createOrthographic( visibleSize.width , visibleSize.height, 0, 1000 );
    //m_Camera->setCameraFlag( CameraFlag::USER2);
    //m_Camera->setPosition3D( Vec3(player->getPositionX(),player->getPositionY(),0));
    //this->setCameraMask((unsigned short)CameraFlag::USER2);
    //this->addChild(m_Camera);
    
    
    isShotting = false;
    isDirctionR = false;
    time = 0;
    monsNum = 0;
    dieId = 9999;
    
    bgNode->addChild( brickNode );
    bgNode->addChild( bulletNode );
    bgNode->addChild( manageNode );
    bgNode->addChild( coinNode );
    bgNode->addChild( solidNode );
    bgNode->addChild( rockNode );
    this->addChild(bgNode);
    
    // Buttons
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
    closeItem->setScale( 3 );
    
    closeItem->setPosition(Vec2(origin.x + visibleSize.width - 150 ,origin.y + 100 ));
    
    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);
    
    dirBall = Sprite::create( "CloseNormal.png" );
    dirBall->setScale( 3 );
    dirBall->setPosition(Vec2(origin.x + 70 ,origin.y + 100 ));
    this->addChild(dirBall);
    
    dirBallRoot = Node::create();
    dirBallRoot->setPosition( dirBall->getPosition() );
    this->addChild(dirBallRoot);
    
    shotButton = Sprite::create( "CloseNormal.png" );
    shotButton->setScale( 3 );
    shotButton->setPosition(Vec2(origin.x + visibleSize.width - 50 ,origin.y + 100 ));
    this->addChild(shotButton);
    
    
    // Camera Follow
    follow = Follow::create(player);
    bgNode->runAction( follow );
    //bgNode->setPositionY( -2000 );
    scheduleUpdate();
    
    
    return true;
}


void HelloWorld::update( float dt )
{
    // use fixed time and calculate 3 times per frame makes physics simulate more precisely.
    //这里表示先走3步瞧瞧  如果fps是1/60  三个setp就是1/180
    if(time % 20 == 0 && time > 120 )
        posYbefore = player->getPositionY();
    //log("posY before = %f" ,posYbefore );
    
    if(time % 20 == 10 && time > 120 )
        posYNow = player->getPositionY();
    //log("posY now = %f" ,posYbefore );
    //log("----------%f",posYNow - posYbefore );
    
    
    for (int i = 0; i < 3; ++i)
    {
        
        m_world->step(1/60.0f);
    }
    
    //log("bgNode Y = %f , player Y = %f ",bgNode->getPositionY() , player->getPositionY() );
    if( bgNode->getPositionY() < 10 && bgNode->getPositionY() > -2100 )
    {
        log("bg Y = %f",bgNode->getPositionY() );
        //if(bgNode->getActionByTag( 1 )->is)
        //bgNode->runAction( follow );
        //follow->setTag( 101 );
       
       // bgNode->setPositionY( bgNode->getPositionY() + (posYNow - posYbefore) );
    }
    else
    {
        //bgNode->stopActionByTag( 101 );
    }
    
    Point playerBefore  = player->getPosition();
    
    // manage player
    if( player->getState()== Ninja::State::WalkRight )
    {
        MoveBy* move = MoveBy::create( 0.1, Vec2( 4 , 0) );
        player->runAction(move);
        //player->setPositionX( player->getPositionX() + 6 );
        //player->getPhysicsBody()->applyImpulse( Vec2( 1 ,0));
    }
    else if (player->getState()== Ninja::State::WalkLeft )
    {
        MoveBy* move = MoveBy::create( 0.1, Vec2( -4 , 0) );
        player->runAction(move);
        //player->setPositionX( player->getPositionX() - 6 );
    }
    else
        player->stopAllActions();
    
    Point delta = player->getPosition() - playerBefore ;
    
    //bgNode->setPosition( bgNode->getPosition() + delta );
    
    if( isShotting )
        shot();
    
    // gun hit
    auto arrBullet = bulletNode->getChildren();
    for( auto &it : arrBullet )
    {
        // monster get hit
        for( int j = 0 ; j < monsNum ; ++j )
        if ( it->getBoundingBox().intersectsRect( monsters[ j ]->getBoundingBox() ) && monsters[ j ]->getState() !=  Ninja::State::None )
        {
            //log("shot !!!!!");
            shake( 8 );
            monsters[ j ]->stopAllActions();
            monsters[ j ]->SetState(Ninja::State::None);
            monsterDie( j );
        }
        
        // rock get hit
        auto arrRock = rockNode->getChildren();
        for( auto &jt: arrRock )
        {
            if( it->getBoundingBox().intersectsRect( jt->getBoundingBox() ) )
            {
                shake( 8 );
                Point *pt = ( Point* )( jt->getUserData() );
                //log("---------%f %f",(*pt).x ,(*pt).y );
                rockLayer->getTileAt( *pt )->setVisible( false );
                jt->getPhysicsBody()->removeFromWorld();
                rockNode->removeChild( jt );
            }
        }
        
    }
    
    // manageMonster
    if( monsNum > 0 )
    {
        for(int i = 0 ; i < monsNum ; ++i )
        {
            float xl = -9999;
            float xr = 9999;
            
            auto arrManage = manageNode->getChildren();
            for( auto &it : arrManage )
            {
                 if( fabs( monsters[ i ]->getPositionY() - it->getPositionY() ) < 10 )
                 {
                     float deltaX = it->getPositionX() - monsters[ i ]->getPositionX();
                     if( deltaX > 0 )
                     {
                         if ( deltaX <= xr - monsters[ i ]->getPositionX() )
                         {
                             xr = it->getPositionX();
                         }
                     }
                     else
                     {
                         deltaX  = deltaX * (-1);
                         if ( deltaX <= monsters[ i ]->getPositionX() - xl )
                         {
                             xl = it->getPositionX();
                         }
                     }
                 }
            }
            if( xl != -9999.0 && xr != 9999.0 )
            {
                log("monster[ %d ] , xl = %f , xr = %f" , i , xl, xr);
                monsters[ i ]->update( xl ,xr );
            }
            monsters[ i ]->manageMonster();
            
            
            if( monsters[ i ]->getState() == Ninja::State::WalkRight )
            {
                MoveBy* move = MoveBy::create( 0.1, Vec2( 1.5 , 0) );
                monsters[ i ]->runAction(move);
            }
            if( monsters[ i ]->getState() == Ninja::State::WalkLeft )
            {
                MoveBy* move = MoveBy::create( 0.1, Vec2( -1.5 , 0) );
                monsters[ i ]->runAction(move);
            }
            
        }
    }
    
    // get Coin
    getCoin();
    
    // addMonster
    if( time % 2 == 0 && monsNum < 12)
    {
        addMonster(monsNum);
        ++monsNum;
    }
    
    ++time;

}

void HelloWorld::shot()
{
    auto bullet1 = Sprite::create("bullet.png");
    bullet1->setScale(2);
    bullet1->setPosition( player->getPosition() );
    
    auto bullet2 = Sprite::create("bullet.png");
    bullet2->setScale(2);
    bullet2->setPosition( player->getPosition() );
    
    
    auto bullet3 = Sprite::create("bullet.png");
    bullet3->setScale(2);
    bullet3->setPosition( player->getPosition() );
    
    
    bulletNode->addChild( bullet1 );
    bulletNode->addChild( bullet2 );
    bulletNode->addChild( bullet3 );
    
    if ( !isDirctionR )
    {
        bullet1->setScaleX(-2);
        bullet2->setScaleX(-2);
        bullet3->setScaleX(-2);
        
        bullet2->setRotation(-5);
        bullet3->setRotation(5);
        
        MoveBy* move1 = MoveBy::create( 1 , Vec2( -1000 , 0) );
        bullet1->runAction(move1);
        
        MoveBy* move2 = MoveBy::create( 1 , Vec2( -1000/cos(5) , 500*sin(5) ) );
        bullet2->runAction(move2);
        
        MoveBy* move3 = MoveBy::create( 1 , Vec2( -1000/cos(5) , -500*sin(5)) );
        bullet3->runAction(move3);
        
        //log("shot Left");
        
        player->setPositionX( player->getPositionX()+2 );
        shake(1);
        
    }
    else
    {
        bullet2->setRotation(5);
        bullet3->setRotation(-5);
        
        MoveBy* move1 = MoveBy::create( 1 , Vec2( 1000 , 0) );
        bullet1->runAction(move1);
        
        MoveBy* move2 = MoveBy::create( 1 , Vec2( 1000/cos(5) , 500*sin(5)) );
        bullet2->runAction(move2);
        
        MoveBy* move3 = MoveBy::create( 1 , Vec2( 1000/cos(5) , -500*sin(5)) );
        bullet3->runAction(move3);
        
        //log("shot Right");
        player->setPositionX( player->getPositionX()-2 );
        shake(1);
    }

}

void HelloWorld::dig()
{
    auto arrSolid = solidNode->getChildren();
    Point pos = player->getPosition();
    for( auto &it: arrSolid )
    {
        //log( "@@@@%f " , it->getPosition().distance( pos ));
        if( it->getPosition().distance( pos ) < 60 )
        {
            if(it->getPhysicsBody())
            {
                Point *pt = ( Point* )(it->getUserData());
                //log("---------%f %f",(*pt).x ,(*pt).y );
                solidLayer->getTileAt( *pt )->setVisible( false );
                it->getPhysicsBody()->removeFromWorld();
                solidNode->removeChild( it );
            }
        }
    }
}

void HelloWorld::getCoin()
{
    auto arrCoin = coinNode->getChildren();
    Point pos = player->getPosition();
    for( auto &it: arrCoin )
    {
        //log( "@@@@%f " , it->getPosition().distance( pos ));
        if( it->getPosition().distance( pos ) < 35 )
        {
            
            Point *pt = ( Point* )(it->getUserData());
            //log("---------%f %f",(*pt).x ,(*pt).y );
            coinLayer->getTileAt( *pt )->setVisible( false );
            //it->getPhysicsBody()->removeFromWorld();
            coinNode->removeChild( it );
            
        }
    }
}

void  HelloWorld::onTouchesBegan( const std::vector<cocos2d::Touch*>& touches, Event *unused_event )
{
    for( auto &it : touches)
    {
        Point pos = it->getLocation();
        if ( isDirBall( pos ) )
        {
            dirBall->setPosition( pos );
            if ( pos.x > dirBallRoot->getPositionX() )
            {
                player->SetState(Ninja::State::WalkRight);
                isDirctionR = true;
            }
            else
            {
               player->SetState(Ninja::State::WalkLeft);
               isDirctionR = false;
            }
            
            if ( dirBallRoot->getPositionY() - dirBall->getPositionY() > 5 )
            {
                dig();
            }
            
            
        }
        if ( isShotButton( pos ) )
        {
            isShotting = true;
        }
    }
    
}

void  HelloWorld::onTouchesMoved( const std::vector<cocos2d::Touch*>& touches, Event *unused_event )
{
    for( auto &it : touches)
    {
        Point pos = it->getLocation();
        if ( isDirBall( pos ) )
        {
            dirBall->setPosition( pos );
            if ( pos.x > dirBallRoot->getPositionX() )
            {
                player->SetState(Ninja::State::WalkRight);
                isDirctionR = true;
            }
            else
            {
                player->SetState(Ninja::State::WalkLeft);
                isDirctionR = false;
            }
            if ( dirBallRoot->getPositionY() - dirBall->getPositionY() > 6 )
            {
                dig();
            }
        }
    }
//    Point pos = touches[0]->getLocation();
//    //log("touch.x = %f ,touch.y = %f" , pos.x , pos.y );
//    if ( pos.x > player->getPositionX() &&  player->getState()==Ninja::State::WalkLeft )
//        player->SetState(Ninja::State::None);
//    if ( pos.x < player->getPositionX() &&  player->getState()==Ninja::State::WalkRight )
//        player->SetState(Ninja::State::None);
    
}

void HelloWorld::onTouchesEnded( const std::vector<cocos2d::Touch*>& touches, Event *unused_event )
{
    player->SetState(Ninja::State::None);
    //shake();
    isShotting = false;
    
    dirBall->setPosition( dirBallRoot->getPosition() );
    
}

const cocos2d::Vec2 HelloWorld::tiledToVec( int i ,int j )const
{
    float X = i * 32.0  ;
    float Y = tiledMap->getContentSize().height - j * 32.0 - 32.0 ;
    return Vec2( X , Y );
}

const cocos2d::Vec2 HelloWorld::vecToTiled( const cocos2d::Vec2 position )const
{
    
    return Vec2( 0 , 0 );
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    if( isCollide( player ) )
    {
        playerBody->applyImpulse( Vec2( 0 , 5500 ) );
        log("jump");
    }
}


void HelloWorld::shake( float f )
{
    this->runAction(Shake::create(0.1f, f ));
}

bool HelloWorld::isDirBall( const cocos2d::Vec2 position )
{
    if( position.getDistance( dirBallRoot->getPosition() ) < 50 )
    {
        //log("dirBall");
        return true;
    }
    else
        return false;
}

bool HelloWorld::isShotButton( const cocos2d::Vec2 position )
{
    if( position.getDistance( shotButton->getPosition() ) < 50 )
    {
        log("shotButton");
        return true;
    }
    else
        return false;
}

void HelloWorld::addMonster( int id )
{
    monsters[ id ]->setVisible( true );
    monsters[ id ]->SetState( Ninja::State::WalkRight);
    auto monsterBody = PhysicsBody::createBox( Size(35.0,50.0) ,PhysicsMaterial(0.0f,0.5f,0.0f));
    monsterBody->setMass( 0.5 );
    monsterBody->setRotationEnable(false);
    monsterBody->setDynamic( true );
    monsters[ id ]->setPhysicsBody( monsterBody );
}

void HelloWorld::monsterDie( int id )
{
    if( monsters[ id ]->getPhysicsBody() )
    {
        monsters[ id ]->getPhysicsBody()->applyImpulse( Vec2( -100 , 100 ) );
        //monsters[ id ]->getPhysicsBody()->removeFromWorld();
        auto scheduler = Director::getInstance()->getScheduler();
        //log("******%d",id);
        //std::string name = "xiaota";
        //scheduler->schedule(CC_CALLBACK_1(HelloWorld::dieCallback,this,&id,id),this,0.1f,-1,1,false,"hello");
        
        
        //monsters[ id ]->
//        runAction( Sequence::create( DelayTime::create( 1.0f ) , CallFunc::create([=](){    monsters[ id ]->getPhysicsBody()->removeFromWorld();
//            monsters[ id ]->setRotation(-90);
//            monsters[ id ]->runAction( MoveBy::create( 3 / 60.0 , Vec2(0 , -3 )));
//            
//        }), nullptr) );
        
         if( isCollide( monsters[ id ] ) )
         {
             monsters[ id ]->getPhysicsBody()->removeFromWorld();
             monsters[ id ]->setRotation(-90);
             monsters[ id ]->runAction( MoveBy::create( 3 / 60.0 , Vec2(0 , -5 )));
         }
        
    }
}

bool HelloWorld::isCollide( Ninja* &ninja )
{
    auto arrBrick = brickNode->getChildren();
    for( auto &it : arrBrick )
    {
        if( it->getPosition().distance( ninja->getPosition() ) < 70   && ( it->getPositionY() < ninja->getPositionY() )  )
           {
               float  X = fabsf( it->getPositionX() - ninja->getPositionX() ) ;
               if( X < 20 )
               {
                   log("@@@ninja on floor ");
                   return true;
               }
               
           }
    }
    
    for( int i = 0 ; i < 12 ; ++i  )
    {
        if(  ninja != monsters[ i ] )
        {
            if( monsters[ i ]->getPosition().distance( ninja->getPosition() ) <  70  && ( monsters[ i ]->getPositionY() < ninja->getPositionY()) )
            {
                float  X = fabsf( monsters[ i ]->getPositionX() - ninja->getPositionX() ) ;
                if( X < 30 )
                {
                    log("@@@ninja on monsters[ %d ] ", i );
                    return true;
                }
            }
        }
    }
    
    return false;
}

