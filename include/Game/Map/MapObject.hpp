
// MapObject.hpp

#pragma once

#include "Containers.hpp"

#include <SDL2/SDL.h>


enum class MapObjType: uint8_t {
    BARRIER = 0,
    TRAP,
    TEXTURE
};


class MapObject {

protected:

    PointF position;
    Point size;

    // rendering
    int texture_id;
    SDL_Texture* texture;

    // object type identifier to avoid dynamic casting
    MapObjType type;

public:

    MapObject(MapObjType t) : 
        type(t),
        position({ 0.0f, 0.0f }),
        size({ 0, 0 }) {}
    MapObjType getType() const;

    virtual void render(SDL_Renderer*) = 0;

    virtual void setPosition(float x, float y);
    virtual void setDimensions(int w, int h);
    virtual void setTextureId(int id);
    virtual void setTexture(SDL_Texture*);
    virtual int getTextureId() const;
    virtual PointF getPosition() const;
    virtual int getWidth() const;
    virtual int getHeight() const;

};

inline MapObjType MapObject::getType() const {
    return type;
}

inline void MapObject::setPosition(float x, float y) {
    this->position = {x, y};
}

inline void MapObject::setDimensions(int w, int h) {
    this->size = {w, h};
}

inline void MapObject::setTextureId(int id ) {
    this->texture_id = id;
}

inline void MapObject::setTexture(SDL_Texture* t) {
    this->texture = t;
}

inline int MapObject::getTextureId() const {
    return texture_id;
}

inline PointF MapObject::getPosition() const {
    return position;
}

inline int MapObject::getWidth() const {
    return size.x;
}

inline int MapObject::getHeight() const {
    return size.y;
}