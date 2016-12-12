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
#include <lumpy/ptree/ptree.h>

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

    cstring         key()   const       { return key_;      }
    const JNode&    value() const       { return *(this+1); }
    JNode&          value()             { return *(this+1); }

    const JKeyVal*  next() const        { return static_cast<const JKeyVal*>(JNode::next());                }
    JKeyVal*        next()              { return static_cast<JKeyVal*>(JNode::next());                      }
    void            next(JKeyVal* node) { next_ = (static_cast<JNode*>(node)-static_cast<JNode*>(this));    }

  protected:
    cstring key_;
};

void sformat(IStringBuffer& buffer, const JNode& node, const FormatSpec& spec);


/* --- proxy --- */
struct ITree;
struct ArrayTree;
struct ObjectTree;

class  EUnexpect: public IException
{};

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

    void operator>>(std::string& value) const;

    void operator<<(const std::string& value);

    struct ArrayItr
    {
      public:
        ArrayItr(const JNode* root)
                : root_((JArray*)root)
        {
            if (root_->type() != JType::Array)  throw EUnexpect{};
        }

        ArrayItr(IBuffer<JNode>* buffer, JNode* root)
                : buffer_(buffer),  root_(static_cast<JArray*>(root))
        {
            if (root_->type() == JType::Null)   root_->type(JType::Array);
            if (root_->type() != JType::Array)  throw EUnexpect{};
        }

        ArrayItr& operator++() {
            addItem(JNull{});
            return *this;
        }

        const ArrayItr& operator++() const {
            if (node_ == nullptr)   node_ = root_+1;
            else                    node_ = node_->next();
            return *this;
        }

        ITree   operator*()         { return {buffer_, root_, node_}; }
        ITree   operator*() const   { return {buffer_, root_, node_}; }

      protected:
        IBuffer<JNode>* buffer_ = nullptr;
        JArray*         root_   = nullptr;
        mutable JNode*  node_   = nullptr;

        void checkType() {
            if (node_->type() != JType::Array){ throw EUnexpect{};}
        }

        template<class Json>
        JNode* addItem(const Json& value) {
            root_->size(+1);
            JNode* node  = &(buffer_->push(value));
            if (node_!=nullptr) node_ ->next(node);

            node_ = node;
            return node_;
        }
    };

    struct ObjectItr
    {
      public:
        ObjectItr(IBuffer<JNode>* buffer, JNode* root): buffer_(buffer),  root_((JObject*)root) {
            if (root_->type() == JType::Null)   root_->type(JType::Object);
            if (root_->type() != JType::Object) throw EUnexpect{};
        }

        ObjectItr(const JNode* root) : buffer_(nullptr), root_((JObject*)(root)) {
            if (root_->type() != JType::Object) throw EUnexpect{};
        }

        ObjectItr& operator[](cstring str) {
            addItem(str, ::strlen(str), JNull{});
            return *this;
        }

        const ObjectItr& operator[](cstring str) const {
            if (node_ == nullptr)   node_ = reinterpret_cast<JKeyVal*>(root_+1);
            else                    node_ = node_->next();
            if (match(str)) return *this;

            node_ = reinterpret_cast<JKeyVal*>(root_+1);
            for(uint i = 0; i < root_->size(); ++i, node_ = node_->next()) {
                if (match(str)) return *this;
            }
            node_ = nullptr;
            return *this;
        }

        ITree   operator*()         { return {buffer_, root_, &(node_->value()) }; }
        ITree   operator*() const   { return {buffer_, root_, &(node_->value()) }; }

        operator bool() const       { return node_ != nullptr; }

      protected:
        IBuffer<JNode>* buffer_ = nullptr;
        JObject*        root_   = nullptr;
        mutable JKeyVal*node_   = nullptr;

        template<class Json>
        JNode* addItem(cstring id, uint size, const Json& val) {
            root_->size(+1);
            auto node = &(buffer_->push(JKeyVal{id, uint16_t(size)}));
            if (node_!=nullptr) node_ ->next(node);
            node_ = node;

            JNode* value = &(buffer_->push(val));
            return value;
        }

        bool match(cstring str) const {
            if (node_         == nullptr)       return false;
            if (node_->type() !=JType::KeyVal)  throw EUnexpect{};
            auto test = strncmp(node_->key(), str, node_->size());
            return test == 0;
        }
    };

    ArrayItr    array() const   { return { node_         }; }
    ArrayItr    array()         { return { buffer_, node_}; }
    ObjectItr   object() const  { return { node_         }; }
    ObjectItr   object()        { return { buffer_, node_}; }

    auto        size()  const   { return node_->size();     }
    operator    bool()  const   { return node_!=nullptr;    }

  protected:
    IBuffer<JNode>* buffer_;
    JNode*          root_   = nullptr;
    JNode*          node_   = nullptr;
};

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
    JTree  jproxy(size);
    auto   oproxy = ptree::proxy(value);
    oproxy.serialize(jproxy);
    return jproxy;
}

template<class T>
void deserialize(const JNode& json, T& value) {
    ITree  jproxy(nullptr, nullptr, const_cast<JNode*>(&json));
    auto   oproxy = ptree::proxy(value);
    oproxy.deserialize(jproxy);
}

template<class T>
void deserialize(const JTree& jtree, T& value) {
    deserialize(jtree.value(), value);
}

}

using json::JTree;

}  // lumpy
