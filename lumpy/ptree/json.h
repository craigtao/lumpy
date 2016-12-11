/*!
 * lumpy is a common c++1z library, mayby the missing library for you.
 * Copyright (C) 2016 lumpy.zhu@gmail.com
 *
 * This program is free software:
 *   you can redistribute it and/or modify it under GPLv3 (or any later version).
 *
 * This program is destributed in the hope that is will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You shold have received a copy of the GNU General Public License alone with the program.
 * If not, see <http://www.gnu.org/licenses/>
 */
#pragma once

#include <lumpy/core.h>

namespace lumpy
{

namespace json {

enum class JType: uint16_t {
    /* basic */
    Null    = 0x0,
    True    = 0x1,
    False   = 0x2,

    Number  = 0x3,
    String  = 0x4,
    KeyVal  = 0x5,

    /* container */
    Array   = 0x6,
    Object  = 0x7,
};

struct JNode
{
    constexpr JNode(JType type)           : type_(type) {}
    constexpr JNode(JType type, uint size): type_(type), size_(uint16_t(size)){}

    constexpr auto  type()      const   { return type_;  }
    void            type(JType value)   { type_ = value; }

    constexpr auto  size()      const   { return size_;  }
    void            size(uint value)    { size_ += value;}

    const JNode*    next()      const   { return next_ == 0 ? nullptr : this+next_; }
    JNode*          next()              { return next_ == 0 ? nullptr : this+next_; }
    void            next(JNode* node)   { next_ = node-this;}

    const JNode*    elements()  const   { return this+1;    }
    JNode*          elements()          { return this+1;    }

  protected:
    JType       type_  = JType::Null;
    uint16_t    size_  = 0;
    int32_t     next_  = 0;
};

struct JNull:   JNode { JNull():    JNode{ JType::Null  } {}};
struct JTrue:   JNode { JTrue():    JNode{ JType::True  } {}};
struct JFalse:  JNode { JFalse():   JNode{ JType::False } {}};
struct JArray:  JNode { JArray():   JNode{ JType::Array } {} };
struct JObject: JNode { JObject():  JNode{ JType::Object} {} };

struct JNumber: JNode {
    JNumber(double  number): JNode{ JType::Number},value_(number){}

    double  value()  const      { return value_; }
    void    value(double value) { value_ = value;}
  protected:
    double  value_;
};
struct JString: JNode {
    JString(cstring value, uint size): JNode{ JType::String, size}, value_(value){}

    cstring value() const                   { return value_;                }
    void    value(cstring value, uint size) { value_ = value; size_ = size; }
  protected:
    cstring value_;
};

struct JKeyVal: JNode {
    JKeyVal(cstring key, uint16_t size): JNode{ JType::KeyVal, size}, key_(key) {}

    cstring         key()   const   { return key_;      }
    const JNode&    value() const   { return *(this+1); }
    JNode&          value()         { return *(this+1); }

    const JKeyVal*  next() const    { return static_cast<const JKeyVal*>(JNode::next());    }
    JKeyVal*        next()          { return static_cast<JKeyVal*>(JNode::next());          }

  protected:
    cstring key_;
};

void sformat(IStringBuffer& buffer, const JNode& node, const FormatSpec& spec);


/* --- proxy --- */
struct ITree;
struct ArrayTree;
struct ObjectTree;

class  EUnexpect{};

struct ITree
{
    ITree(IBuffer<JNode>* buffer, JNode* root, JNode* node): buffer_(buffer), root_(root), node_(node){}

    template<class T, class=If<isReal<T>> >
    void operator>>(T& value) const {
        if (node_->type() != JType::Number)  { throw EUnexpect();  }
        auto node = reinterpret_cast<JNumber*>(node_);
        value = T(node->value());
    }

    template<class T, class=If<isReal<T>>>
    void operator<<(const T& value) {
        if (node_->type() == JType::Null)    { node_->type(JType::Number); buffer_->grow(1);    }
        if (node_->type() != JType::Number)  { throw EUnexpect();  }
        auto node = reinterpret_cast<JNumber*>(node_);
        node->value(double(value));
    }

    ArrayTree   array() const;
    ArrayTree   array();

    ObjectTree  object() const;
    ObjectTree  object();

    auto        size() const { return node_->size(); }

  protected:
    IBuffer<JNode>* buffer_;
    JNode*          root_   = nullptr;
    JNode*          node_   = nullptr;

    template<class Json>
    JNode* addItem(const Json& value) {
        if (root_!=nullptr) root_->size(1);
        JNode* node  = &(buffer_->push(value));
        node_ ->next(node);
        node_ = node;
        return node_;
    }

    template<class Json>
    JNode* addItem(cstring key, uint size, const Json& value) {
        addItem(JKeyVal{key, uint16_t(size)});
        JNode* node = &(buffer_->push(value));
        return node;
    }
};


struct ArrayTree: ITree
{
    ITree operator[](uint idx) {
        auto node = addItem(JNull{});
        return {buffer_, root_, node};
    }

    ITree operator[](uint idx) const {
        return {buffer_, root_, root_==node_ ? root_+1 : node_->next()};
    }

  private:
    friend struct ITree;
    ArrayTree(IBuffer<JNode>* buffer, JNode* root, JNode* node): ITree(buffer, root, node) {}
};

struct ObjectTree: ITree
{
    ITree operator[](cstring idx) {
        auto node = addItem(idx, strlen(idx), JNull{});
        return {buffer_, root_, node};
    }

    ITree operator[](cstring idx) const {
        auto keynode = reinterpret_cast<JKeyVal*>(node_->next());
        if (strncmp(keynode->key(), idx, keynode->size())==0) {
            return { buffer_, root_, &(keynode->value())};
        }
        keynode = reinterpret_cast<JKeyVal*>(root_+1);
        for(uint16_t i = 0; i < root_->size(); ++i, keynode=keynode->next()) {
            if (strncmp(keynode->key(), idx, keynode->size())==0) {
                return { buffer_, root_, &(keynode->value())};
            }
        }
        return {buffer_, root_, nullptr};
    }

  private:
    friend struct ITree;
    ObjectTree(IBuffer<JNode>* buffer, JNode* root, JNode* node): ITree(buffer, root, node) {}
};

inline ArrayTree ITree::array() {
    if (node_->type() == JType::Null) { node_->type(JType::Array);  }
    if (node_->type() != JType::Array){ throw EUnexpect{};          }
    return { buffer_, node_, node_};
}

inline ArrayTree ITree::array() const {
    if (node_->type() != JType::Array){ throw EUnexpect{};          }
    return { buffer_, node_, node_};
}

inline ObjectTree ITree::object() {
    if (node_->type() == JType::Null)  { node_->type(JType::Object);}
    if (node_->type() != JType::Object){ throw EUnexpect{};         }
    return { buffer_, node_, node_};
}

inline ObjectTree ITree::object() const {
    if (node_->type() != JType::Object){ throw EUnexpect{};         }
    return { buffer_, node_, node_};
}

class EJParseFailed : public IException
{
public:
    EJParseFailed(cstring s, uint pos): str_(s), pos_(pos) {}

    void sformat(IStringBuffer& buffer, const FormatSpec& spec) const override {
        auto beg = pos_ > 10 ? pos_-10 : 0;
        buffer.formats(" ...'{}'...", string(str_+beg, pos_-beg));
    }
protected:
    cstring str_;
    uint    pos_;
};

class JTree
{
  public:
    JTree(size_t size=4*1024*1024) : buffer_{size}, proxy_{&buffer_, nullptr, &buffer_[0]} {
        buffer_.push(JNull{});
    }

    JTree(cstring str, size_t size) : buffer_{size}, proxy_{&buffer_, nullptr, &buffer_[0]} {
        parse(str, size);
    }

    JNode&       value()        { return buffer_[0]; }
    const JNode& value() const  { return buffer_[0]; }

    auto object()       { return proxy_.object();   }
    auto array()        { return proxy_.array();    }
    auto object() const { return proxy_.object();   }
    auto array()  const { return proxy_.array();    }

    auto size() const   { return proxy_.size();     }

    //~ for parse only
    template<class T>
    T& add_item(JNode* proot, JNode* pprev, const T& item) {
        auto pnode = &(buffer_.push(item));
        if (proot!=nullptr) proot->size(+1);
        if (pprev!=nullptr) pprev->next(pnode);
        return *pnode;
    }

  private:
    Buffer<JNode>   buffer_;
    ITree           proxy_;

    static void staticInit();
    void parse(cstring str, size_t size);
};

inline void sformat(IStringBuffer& buffer, const JTree& tree, const FormatSpec& spec) {
    sformat(buffer, tree.value(), spec);
}

template<class T>
JTree serialize(T& value, size_t size=4*1024*1024) {
    JTree proxy(size);
    auto   ptree = value.ptree();
    ptree.serialize(proxy);
    return proxy;
}

template<class T>
void deserialize(const JNode& json, T& value) {
    ITree  proxy(nullptr, nullptr, const_cast<JNode*>(&json));
    auto   ptree = value.ptree();
    ptree.deserialize(proxy);
}

template<class T>
void deserialize(const JTree& jtree, T& value) {
    deserialize(jtree.value(), value);
}

}

using json::JTree;

}  // lumpy
