#ifndef SceneObject_h
#define SceneObject_h


namespace JTTW {
class SceneObject : public cocos2d::Sprite {
public:
    virtual double getYRange() = 0;
};
}

#endif /* SceneObject_h */
