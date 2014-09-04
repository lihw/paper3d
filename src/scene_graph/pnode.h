// pnode.h
// Every object in a scene is a node. A node has transformation interface and
// hierachical organization.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//
// 

#ifndef PNODE_H
#define PNODE_H

#include <PFoundation/pobject.h>
#include <PFoundation/ppropertystring.h>
#include <PFoundation/ppropertytransform.h>
#include <PFoundation/pstringmap.h>
#include <PFoundation/plist.h>
#include <PFoundation/pbox.h>

class PScene;

class P_DLLEXPORT PNode: public PObject
{
    P_OBJECT

    friend class PScene;

    PNode(const PNode &other) {}
    void operator=(const PNode &other) {}

public:
    PNode(const pchar *name, PScene *scene, PNode* parent = P_NULL, pbool isDrawable = false);
    virtual ~PNode();

    P_INLINE PPropertyTransform& transform()            { return m_localTransform; }
    P_INLINE const PMatrix4x4 &worldTransform() const   { return m_worldTransform; }
    P_INLINE puint32 numberOfChildren() const           { return m_children.count(); }
    P_INLINE PNode *parent() const                      { return m_parent; }
    P_INLINE PScene *scene() const                      { return m_scene; }
    P_INLINE const PString &name() const                { return m_name.toString(); }
    P_INLINE pbool isDrawable() const                   { return m_drawable; }
    P_INLINE pbool worldTransformChanged() const        { return m_worldTransformChanged; }

    PNode *child(const pchar *name) const;
    void setParent(PNode *parent); 
    void setName(const char *name);
    
    virtual pbool unpack(const PXmlElement* xmlElement);

    class iterator
    {
    public:
        PNode *m_root;

        P_INLINE iterator(PNode *root)    
        { 
            m_current = root;
        }

        P_INLINE PNode *operator*() const { return m_current; }

        P_INLINE iterator& operator++()
        {
            PStringMap<PNode *>::iterator it = m_current->m_children.begin();
            PStringMap<PNode *>::iterator end = m_current->m_children.end();

            while (it != end)
            {
                m_queue.append(it.value());
                ++it;
            }

            if (m_queue.isEmpty())
            {
                m_current = P_NULL;
            }
            else
            {
                m_current = m_queue.front();
                m_queue.pop_front();
            }

            return *this;
        }

    private:
        PList<PNode *>  m_queue;
        PNode          *m_current;
    };

protected:
    // Called right after the child is added.
    virtual void onChildAdded(PNode *child);
    // Called right before the child is to be removed.
    virtual void onChildRemoved(PNode *child);
    // Update node's own data and doesn't do any actual rendering.
    virtual void update(); 
    virtual void pause();
    virtual void resume();
    virtual void updateWorldTransform();
    virtual void updateBBox();

private:
    void addChild(PNode *child);
    void removeChild(const pchar *name);

protected:
    PPropertyString       m_name;
    pbool                 m_drawable;       // Whether this object is a drawable one.
    PNode                *m_parent;         // Reference to parent pobject
    PStringMap<PNode *>   m_children;       // array of children pobjects
    PMatrix4x4            m_worldTransform; // Local space -> world space.
    PPropertyTransform    m_localTransform; // loal transformation
    PScene               *m_scene;          // The scene the drawable belongs to
    pbool                 m_worldTransformChanged; // If the world transform changed in the current frame.
    PBox                  m_bbox;
};

#endif // !PNODE_H

