/*****************************************************************************

  Licensed to Accellera Systems Initiative Inc. (Accellera) under one or
  more contributor license agreements.  See the NOTICE file distributed
  with this work for additional information regarding copyright ownership.
  Accellera licenses this file to you under the Apache License, Version 2.0
  (the "License"); you may not use this file except in compliance with the
  License.  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
  implied.  See the License for the specific language governing
  permissions and limitations under the License.

 *****************************************************************************/

#include "utils_cfg.h"

//using namespace hmesl;

// construct function
utils_config::utils_config(string filename)
{
    total_time = 0;
    phase_time = 0;

    if (read_config(filename))
    {
        cout <<"read top configure successful!!" << endl;
    }
}

// destruct function
utils_config::~utils_config()
{
    //cout << "erase configure data ~" << endl;
}



// Read parameter
bool  utils_config::read_config(string filename)
{
    ptree pt;
    read_xml(filename.c_str(), pt, xml_parser::trim_whitespace);   //读入一个xml文件

    //Get Point Value
    //cout << "ID is " << pt.get<int>("con.id") << endl;  //读取节点中的信息

    //Get Default Value
    //cout << "Try Default" << pt.get<int>("con.no_prop", 100) << endl;  //如果取不到，则使用默认值

    //Select One Point
    //ptree child = pt.get_child("con");    //取一个子节点

    //Get String
    //cout << "name is :" << child.get<string>("name") << endl;    //对子节点操作，其实跟上面的操作一样

    //
    //child = pt.get_child("con.urls");

    //Iterator
    //for (ptree::iterator pos = child.begin(); pos != child.end(); ++pos)  //boost中的auto
    //{
    //    cout << "\t" + pos->second.data() << endl;
    //}

    //Set Value
    //pt.put("con.name", "Sword");    //更改某个键值

    //Add Point
    //pt.add("con.urls.url", "http://www.baidu.com"); //增加某个键值

    ptree child = pt.get_child("root.sim");

    //ptree subchild = pt.get_child("root.sim.log_level");//get_child("log_level");
    //获取子节点列表
    //auto Descendants(const wptree & root, const wstring & key) -> decltype(root.get_child_optional(key))
    //{
    //    return root.get_child_optional(key);
    //}

    this->total_time = child.get<int>("<xmlattr>.time", 2000);
    this->phase_time = child.get<int>("<xmlattr>.phase", 200);

    string unit_cfg = child.get<string>("<xmlattr>.unit");

    if (unit_cfg == "us" or unit_cfg == "US") {
        this->sim_unit = SC_US;
    }
    else if (unit_cfg == "ns" or unit_cfg == "NS") {
        this->sim_unit = SC_NS;
    }
    else if (unit_cfg == "ms" or unit_cfg == "MS") {
        this->sim_unit = SC_MS;
    }
    else {
        cout <<"Unknow Simulation Time Unit!!" <<unit_cfg<<"" << endl;
        this->sim_unit = SC_NS;
    }
    //this->

    //Get XML Point attribute
    cout << "sim time number :" << child.get<int>("<xmlattr>.time", 200) << endl;
    cout << "sim time unit   :" << child.get<string>("<xmlattr>.unit", "US") << endl;
    cout << "sim time phase  :" << child.get<int>("<xmlattr>.phase", 1) << endl;


    //
    //child = pt.get_child("root.gen.gen0");

   //Iterator
   /*
   for (ptree::iterator pos = child.begin(); pos != child.end(); ++pos)  //boost中的auto
   {
       //cout << "\t" + pos->second.data() << endl;
       //pos->second.get<int>;
       int tmp = stoi(pos->second.data());
       gen0.push_back(tmp);
   }
   */

    /*
   child = pt.get_child("root.gen.gen1");

   //Iterator
   for (ptree::iterator pos = child.begin(); pos != child.end(); ++pos)  //boost中的auto
   {
       //cout << "\t" + pos->second.data() << endl;
       //pos->second.get<int>;
       int tmp = stoi(pos->second.data());
       gen1.push_back(tmp);
   }
   */

    //Format Write XML
    //xml_writer_settings<std::string> settings('\t', 1);
    //write_xml("conf.xml", pt, std::locale(), settings);    //写入XML

    //getchar();
    return true;
}




