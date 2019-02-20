/*
 * Test access to xtc data for LCLS2
 */
#include <fcntl.h> // O_RDONLY
#include <stdio.h> // for  sprintf, printf( "%lf\n", accum );
#include <iostream> // for cout, puts etc.
//#include <vector>
//#include <stdlib.h>
//#include <fstream>
//#include <stdlib.h>
#include <unistd.h> // close
#include <stdint.h>  // uint8_t, uint32_t, etc.

#include "xtcdata/xtc/XtcFileIterator.hh"
#include "xtcdata/xtc/XtcIterator.hh"
#include "xtcdata/xtc/ShapesData.hh"
#include "xtcdata/xtc/DescData.hh"
#include "xtcdata/xtc/NamesIter.hh"
#include "xtcdata/xtc/NamesLookup.hh"

//using namespace psalgos;
//using namespace psalg;
using namespace std; 

using namespace XtcData;
//using std::string;

//-----------------------------

class TestXtcIterator : public XtcData::XtcIterator
{
public:
    enum { Stop, Continue };
    TestXtcIterator(XtcData::Xtc* xtc) : XtcData::XtcIterator(xtc) {} // {iterate();}
    TestXtcIterator() : XtcData::XtcIterator() {}

    int process(XtcData::Xtc* xtc) {
        TypeId::Type type = xtc->contains.id(); 
	printf("    TestXtcIterator TypeId::%-12s id: %-4d Xtc* pointer: %p\n", TypeId::name(type), type, xtc);

	switch (xtc->contains.id()) {
	case (TypeId::Parent): {break;}
	case (TypeId::Names): {break;}
        case (TypeId::ShapesData): {break;}
        case (TypeId::Data): {break;}
        case (TypeId::Shapes): {break;}
	default: {
	    printf("    WARNING: UNKNOWN DATA TYPE !!! \n");
	    break;
	    }
	}

        //==============
	  iterate(xtc);
        //==============

        return Continue;
    }
};

//-----------------------------

class ConfigIter : public XtcData::NamesIter
{
public:
    ConfigIter(XtcData::Xtc* xtc) : XtcData::NamesIter(xtc) { iterate(); }
    ConfigIter() : XtcData::NamesIter() {}
    // ~ConfigIter() { delete _p; }

    int process(XtcData::Xtc* xtc)
    {
        TypeId::Type type = xtc->contains.id(); 
	printf("ConfigIter TypeId::%-20s Xtc*: %p\n", TypeId::name(type), &xtc);

	switch (xtc->contains.id()) {
	case (TypeId::Parent): {
	  //printf("ConfigIter case TypeId::Parent - keep iterating\n");
	    iterate(xtc); // look inside anything that is a Parent
	    break;
	}
	case (TypeId::Names): {
	  //printf("ConfigIter case TypeId::Names - grab names in namesLookup\n");
	    Names& names = *(Names*)xtc;
	    NamesId& namesId = names.namesId();
            NamesLookup& _namesLookup = namesLookup();
	    _namesLookup[namesId] = NameIndex(names);
	    //printf("ConfigIter case TypeId::Names add number of names: %d namesId: %d\n", names.num(), namesId.namesId());
	    break;
	}
        case (TypeId::ShapesData): {
	    ShapesData* pshapesdata = (ShapesData*)xtc;
            NamesId namesId = pshapesdata->namesId();
            _shapesData[namesId.namesId()] = pshapesdata;
	    //printf("ConfigIter case TypeId::ShapesData): namesId.level: %d value: %d namesId: %d\n",
	    //	   namesId.level(), namesId.value(), namesId.namesId());
            break;
        }
	default:
	    break;
	}
	return Continue;
    }

    ShapesData& shape() {return *_shapesData[0];}
    ShapesData& value() {return *_shapesData[1];}
    //NamesLookup& namesLookup() {return _namesLookup;} // defined in super-class NamesIter
    //void iterate();                                   // defined in super-super-class XtcIterator
    DescData* desc_shape() {return new DescData(shape(), namesLookup()[shape().namesId()]);}
    DescData* desc_value() {return new DescData(value(), namesLookup()[value().namesId()]);}

private:
    ShapesData* _shapesData[2];
};

//-----------------------------

class DataIter : public XtcIterator
{
public:
    enum {Stop, Continue};
    DataIter(Xtc* xtc) : XtcIterator(xtc) { iterate(); }

    int process(Xtc* xtc)
    {
        // enum Type {Parent, ShapesData, Shapes, Data, Names, NumberOf};
        TypeId::Type type = xtc->contains.id(); 
	printf("DataIter TypeId::%-20s Xtc*: %p\n", TypeId::name(type), &xtc);

        switch (type) {
        case (TypeId::Parent): {
	  //printf("YYYY In DataIter TypeId::Parent - keep iterating\n");
	    iterate(xtc); 
            break;
        }
        case (TypeId::Names): {
	  //Names& names = *(Names*)xtc;
	  //Alg& alg = names.alg();
	  //printf("*** DetName: %s, DetType: %s, Alg: %s, Version: 0x%6.6x, Names:\n",
	  //       names.detName(), names.detType(), alg.name(), alg.version());

	  //printf("number of names:  %d\n", names.num());
	  //for (unsigned i = 0; i < names.num(); i++) {
	  //    Name& name = names.get(i);
	  //    printf("%02d XX Name %-32s rank %d type %d\n", i, name.name(), name.rank(), name.type());
	  //}
            break;
        }
        case (TypeId::ShapesData): {
 
	    ShapesData* pshapesdata = (ShapesData*)xtc;
            NamesId namesId = pshapesdata->namesId();
            _shapesData[namesId.namesId()] = pshapesdata;
 
	    //printf("YYYY  case TypeId::ShapesData): namesId.level: %d value: %d namesId: %d\n",
	    //	   namesId.level(), namesId.value(), namesId.namesId());
            break;
        }
        case (TypeId::Shapes): {break;}
        case (TypeId::Data):   {break;}
        default:{cout << "YYYY TypeId::default ????? type = " << type << " \n"; break;}
        }

	//cout << "XXXX In DataIter just before exit\n";
        return Continue;
    }

    ShapesData& shape() {return *_shapesData[0];}
    ShapesData& value() {return *_shapesData[1];}

private:
    ShapesData* _shapesData[2];
};

//-----------------------------

// void dump(const char* transition, Names& names, DescData& descdata) {
void dump(const char* transition, DescData& descdata) {
 
   Names& names = descdata.nameindex().names();

    printf("------ %d Names for transition %s ---------\n", names.num(), transition);
    for (unsigned i = 0; i < names.num(); i++) {
        Name& name = names.get(i);
        printf("%02d name %-32s rank %d type %d\n", i, name.name(), name.rank(), name.type());
    }
    printf("------ Values for transition %s ---------\n",transition);
    for (unsigned i = 0; i < names.num(); i++) {
        Name& name = names.get(i);
        if (name.type()==Name::INT64 and name.rank()==0) {
	  printf("%02d name %-32s value %ld\n", i, name.name(), descdata.get_value<int64_t>(name.name()));
        }
    }

    printf("============= THE END OF DUMP FOR TRANSITION %s =============\n\n", transition);
}

//-----------------------------
//-----------------------------
//-----------------------------
//-----------------------------
//-----------------------------
//-----------------------------
//-----------------------------
//-----------------------------
//-----------------------------

int file_descriptor(int argc, char* argv[]) {

    const char* fname = "/reg/neh/home/cpo/git/lcls2/psana/psana/dgramPort/jungfrau.xtc2";
    std::cout << "xtc file name: " << fname << '\n';

    int fd = open(fname, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Unable to open file '%s'\n", fname);
        exit(2);
    }
    
    return fd;
}

//-----------------------------

int test_xtc_content(int argc, char* argv[]) {

    int fd = file_descriptor(argc, argv);
    XtcFileIterator it_fdg(fd, 0x4000000);
    //Dgram* dg = it_fdg.next();
    Dgram* dg;

    unsigned ndgreq=1000000;
    unsigned ndg=0;
    while ((dg = it_fdg.next())) {
        ndg++;
	printf("%04d ---- datagram ----\n", ndg);
        if (ndg>=ndgreq) break;
        TestXtcIterator iter(&(dg->xtc));
        iter.iterate();
    }
    return 0;
}

//-----------------------------

#define DESC_FOR_METHOD(oDescData, itero, names_map, method) \
        XtcData::DescData oDescData(itero.method(), names_map[itero.method().namesId()])
#define DESC_SHAPE(oDescData, itero, names_map) DESC_FOR_METHOD(oDescData, itero, names_map, shape)
#define DESC_VALUE(oDescData, itero, names_map) DESC_FOR_METHOD(oDescData, itero, names_map, value)

int test_all(int argc, char* argv[]) {

    int fd = file_descriptor(argc, argv);
    XtcFileIterator xfi(fd, 0x4000000);

    Dgram* dg = xfi.next();

    // get data out of the 1-st datagram configure transition

    ConfigIter configo(&(dg->xtc));
    NamesLookup& names_map = configo.namesLookup();

    //NamesId& names_id_shape = configo.shape().namesId();
    //DescData desc_cfg_shapes(configo.shape(), names_map[names_id_shape]);
    //dump("Configure", desc_cfg_shapes);

    //DescData* p_desc_shape = configo.desc_shape();
    //dump("Configure shapes", *p_desc_shape);

    DESC_SHAPE(desc_shape, configo, names_map);
    dump("Config shapes", desc_shape);

    //NamesId& names_id_value = configo.value().namesId();
    //DescData desc_cfg_values(configo.value(), names_map[names_id_value]);
    //dump("Configure values", desc_cfg_values);

    //DescData* p_desc_value = configo.desc_value();
    //dump("Configure values", *p_desc_value);

    DESC_VALUE(desc_value, configo, names_map);
    dump("Config values", desc_value);

    //return 0;

    unsigned neventreq=3;
    unsigned nevent=0;
    while ((dg = xfi.next())) {
        if (nevent>=neventreq) break;
        nevent++;

        DataIter datao(&(dg->xtc));
 
        printf("evt:%04d ==== transition: %s of type: %d time %d.%09d, pulseId %lux, env %ux, "
               "payloadSize %d extent %d\n", nevent,
               TransitionId::name(dg->seq.service()), dg->seq.type(), dg->seq.stamp().seconds(),
               dg->seq.stamp().nanoseconds(), dg->seq.pulseId().value(),
               dg->env, dg->xtc.sizeofPayload(), dg->xtc.extent);

        //NamesId& namesId = iter.value().namesId();
        //DescData desc_data(iter.value(), names_map[namesId]);
        //Names& names = descdata.nameindex().names();
        //dump("Value", desc_data);

        DESC_VALUE(desc_data, datao, names_map);
        dump("Data values", desc_data);
    }

    //delete p_desc_shape;
    //delete p_desc_value;

    ::close(fd);
    return 0;
}

//-----------------------------

int main (int argc, char* argv[]) {
  //return test_xtc_content(argc, argv);
  return test_all(argc, argv);
}

//-----------------------------