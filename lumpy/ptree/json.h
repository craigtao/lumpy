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

namespace ptree
{

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

class EJParseFailed : public IException
{
public:
    EJParseFailed(cstring s, uint pos): str_(s), pos_(pos) {}

    void sformat(IStringBuffer& buffer) const override {
        auto beg = pos_ > 10 ? pos_-10 : 0;
        buffer.formats(" ...'{}'...", string(str_+beg, pos_-beg));
    }
protected:
    cstring str_;
    uint    pos_;
};

struct JNode
{
    constexpr JNode(JType type)               : type(type)            {}
    constexpr JNode(JType type, uint16_t size): type(type), size(size){}

    const JNode* next() const { return right == 0 ? nullptr : this+right; }
    JNode*       next()       { return right == 0 ? nullptr : this+right; }

    JType       type    = JType::Null;
    uint16_t    size    = 0;
    int32_t     right   = 0;
};
struct JNull:   JNode { JNull():    JNode{ JType::Null  } {}};
struct JTrue:   JNode { JTrue():    JNode{ JType::True  } {}};
struct JFalse:  JNode { JFalse():   JNode{ JType::False } {}};
struct JArray:  JNode { JArray():   JNode{ JType::Array } {}};
struct JObject: JNode { JObject():  JNode{ JType::Object} {}};
struct JNumber: JNode { double  value;  JNumber(double  number)              : JNode{ JType::Number},       value(number){}};
struct JString: JNode { cstring string; JString(cstring value, uint16_t size): JNode{ JType::String, size}, string(value){}};
struct JKeyVal: JNode {
    JKeyVal(cstring key, uint16_t size): JNode{ JType::KeyVal, size}, string(key) {}

    const JNode&    value() const   { return *(this+1); }
    JNode&          value()         { return *(this+1); }

    const JKeyVal*  next() const    { return static_cast<const JKeyVal*>(JNode::next());    }
    JKeyVal*        next()          { return static_cast<JKeyVal*>(JNode::next());          }

    cstring string;
};

class  JDoc: Buffer<JNode>
{
  public:
    JDoc(cstring str, size_t size): Buffer<JNode>(size) {
        parse(str, size);
    }

    template<class T>
    JNode& add_item(JNode* proot, JNode* pleft, const T& item) {
        auto pnode = this->push(&item, sizeof(T)/sizeof(JNode));
        if (proot!=nullptr) proot->size  ++;
        if (pleft!=nullptr) pleft->right = int32_t(pnode-pleft);
        return *pnode;
    }

    const JNode& value() { return this->data_[0]; }

  private:
    static void staticInit();
    void parse(cstring str, size_t size);
};

void sformat(IStringBuffer& buffer, const JNode& node, const FormatSpec& spec);

}

using ptree::JDoc;

}
