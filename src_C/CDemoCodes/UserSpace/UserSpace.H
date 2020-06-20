/*
 * UserSpace.h
 *
 *  Created on: Mar 5, 2016
 *      Author: zhanggan
 */

#ifndef USERSPACE_H_
#define USERSPACE_H_
#include <iostream>

namespace std {
typedef char MrdbLibraryName_t[10];
typedef char MrdbObjectName_t[10];

typedef char MrdbRlsName_t[6];
typedef char MrdbPTFName_t[7];
typedef char MrdbDescriptionName_t[50];
typedef char MrdbiASPName_t[10];
typedef char MrdbSystemName_t[10];

#define OBJECT_BLOCKSIZE 1024*1024*16

#define DESC_LEN_50 50
#pragma datamodel(p128)
/*
 * H: Header type for the UserSpace header. We expect there's a field as NumberOfEntries to let us know the size of the userspace by the header.
 * E: Entry type for every entry data in UserSpace. We expect this class could do compare by "=="
 * K: Key value in the Entry, which enable us to save unique data in the list.
 */

template<typename H, typename E>
class UserSpace {
public:
	UserSpace();
	UserSpace(const MrdbObjectName_t *, const MrdbLibraryName_t*);
	UserSpace(const MrdbObjectName_t *, const MrdbLibraryName_t*, const H *,
			const E *);

	virtual ~UserSpace();
	void * getUserSpacePtr();
	//E * getAllEntries(int *);
	int getAllEntries(E *, int, int *);
	int getEntryNumber();
	int addOneEntry(E &);
	int rmvOneEntry(E &);
	int chgOneEntry(E &);
protected:

	void createUserSpace(const H *, const E *);
	void printALL();
	MrdbObjectName_t objectName;
	MrdbLibraryName_t libraryName;
	MrdbDescriptionName_t description;
	void * ptrUserSpace;
};
template<typename H, typename E> void UserSpace<H, E>::printALL() {
	if (ptrUserSpace != NULL) {
		cout <<"==============printALL=============================="<<endl;
		char * p = (char *) ptrUserSpace - sizeof(MrdbObjectName_t) - sizeof(MrdbLibraryName_t);
		H header;
		E entry;
		cout << "ObjName=" << p;
		p += sizeof(MrdbObjectName_t);
		cout << "LibName=" << p;
		cout << endl;
		memcpy((void *) &header, ptrUserSpace, sizeof(H));
		cout << "NumberOfEntries=" << header.NumberOfEntries << endl;
		p = (char *) ptrUserSpace + sizeof(H);
		for (int i = 0; i < header.NumberOfEntries; i++) {
			memcpy((void *) &entry, p, sizeof(E));
			cout << "Entry[" << i << "]=";
			entry.printMe();
			cout<< endl;
		}
	}else{
		cout<<"ptrUserSpace=NULL"<<endl;
	}
	cout <<"====================================================="<<endl;

}
template<typename H, typename E> UserSpace<H, E>::UserSpace() {
	memset(objectName, 0x40, sizeof(MrdbObjectName_t));
	memset(libraryName, 0x40, sizeof(MrdbLibraryName_t));
	ptrUserSpace = NULL;
}

template<typename H, typename E> void UserSpace<H, E>::createUserSpace(
		const H * header, const E * e) {
	void * p = NULL;
	p = (void *) malloc(
			sizeof(MrdbObjectName_t) + sizeof(MrdbLibraryName_t) + sizeof(H)
					+ sizeof(E) * 100);

	memcpy(p, objectName, sizeof(MrdbObjectName_t));
	memcpy((char *) p + sizeof(MrdbObjectName_t), libraryName,
			sizeof(MrdbLibraryName_t));
	ptrUserSpace = (char *) p + sizeof(MrdbObjectName_t)
			+ sizeof(MrdbLibraryName_t);

	if (header != NULL) {
		memcpy(ptrUserSpace, (void *) header, sizeof(H));
	} else {
		//Init the structure.
		H h;
		memset((void *) &h, 0, sizeof(H));
		h.NumberOfEntries = 0;
		memcpy((void *) ptrUserSpace, (void *) &h, sizeof(H));
	}
	if (e != NULL) {
		memcpy((char *) ptrUserSpace + sizeof(H), (void *) e,
				sizeof(E) * header->NumberOfEntries);
	}

}
template<typename H, typename E> UserSpace<H, E>::UserSpace(
		const MrdbObjectName_t *obj, const MrdbLibraryName_t * library) {
	memcpy(objectName, obj, sizeof(MrdbObjectName_t));
	memcpy(libraryName, library, sizeof(MrdbLibraryName_t));
	cout<<"zgCP2"<<endl;
	createUserSpace((H *) NULL, (E *) NULL);//This for the existing UserSpace reading.
	cout<<"zgCP3"<<endl;
}

template<typename H, typename E> UserSpace<H, E>::~UserSpace() {
}
template<typename H, typename E> UserSpace<H, E>::UserSpace(
		const MrdbObjectName_t *obj, const MrdbLibraryName_t * library,
		const H *h, const E * e) {
	memcpy(objectName, obj, sizeof(MrdbObjectName_t));
	memcpy(libraryName, library, sizeof(MrdbLibraryName_t));
	createUserSpace(h, e);		//Re-construct the whole User Space.
}
template<typename H, typename E> void * UserSpace<H, E>::getUserSpacePtr() {
	return ptrUserSpace;
}
template<typename H, typename E> int UserSpace<H, E>::getAllEntries(E *info,
		int size, int * numOfEntries) {

	H header;
	if ( NULL == ptrUserSpace)
		return 0;
	memcpy(&header, ptrUserSpace, sizeof(H));

	*numOfEntries = header.NumberOfEntries;
	if (*numOfEntries <= size) {
		memcpy(info, (char *) ptrUserSpace + sizeof(H),
				sizeof(E) * *numOfEntries);
		return 0;
	} else {
		return -1;
	}
}
template<typename H, typename E> int UserSpace<H, E>::getEntryNumber() {

	H header;
	if ( NULL == ptrUserSpace)
		return 0;
	memcpy(&header, ptrUserSpace, sizeof(H));
	return header.NumberOfEntries;

}
template<typename H, typename E> int UserSpace<H, E>::addOneEntry(E &e) {
	H header;
	void * p;
	if ( NULL == ptrUserSpace)
		return -1; // As we do not have user space created, can not add one entry.
	printALL();
	memcpy((void *) &header, ptrUserSpace, sizeof(H));
	int num = header.NumberOfEntries;
	p = (char *) ptrUserSpace + sizeof(H) + sizeof(E) * num;
	memcpy(p, (void *) &e, sizeof(E));
	header.NumberOfEntries++;
	memcpy(ptrUserSpace, (void *) &header, sizeof(H));
	printALL();
	return 0;
}
template<typename H, typename E> int UserSpace<H, E>::rmvOneEntry(E &e) {
	H header;
	void * p;
	E oneentry;
	bool found = false;
	if ( NULL == ptrUserSpace)
		return 0;	// As we do not have userspace existed, remove is "done".

	memcpy((void *) &header, ptrUserSpace, sizeof(H));
	int num = header.NumberOfEntries;
	p = (char *) ptrUserSpace + sizeof(H);
	int i = 0;
	for (i = 0; i < num; i++) {
		memcpy((void *) &oneentry, p, sizeof(E));
		if (oneentry == e) {
			//TODO: Remove this item.

			break;
		}
		p = (char *) p + sizeof(E);
	}
	if (i < num - 1) {// We only need to copy when we still have more items after p.
	//We would ignore the extra duplicate data after the NumberOfEntries.
		memcpy(p, (char *) p + sizeof(E), (num - i) * sizeof(E));
	}
	header.NumberOfEntries--;
	memcpy(ptrUserSpace, (void *) &header, sizeof(H));

	return 0;
}
template<typename H, typename E> int UserSpace<H, E>::chgOneEntry(E &e) {
	H header;
	void * p;
	E oneentry;
	bool found = false;
	if ( NULL == ptrUserSpace)
		return 0;	// As we do not have userspace existed, remove is "done".
	memcpy((void *) &header, ptrUserSpace, sizeof(H));
	int num = header.NumberOfEntries;
	p = (char *) ptrUserSpace + sizeof(H);

	for (int i = 0; i < num; i++) {
		memcpy((void *) &oneentry, p, sizeof(E));
		if (oneentry == e) {

			break;
		}
		p = (char *) p + sizeof(E);
	}
	memcpy(p, (void *) &e, sizeof(E)); /*To cover the current content with the new entry input*/
	return 0;
}

#pragma datamodel(pop)
} /* namespace std */

#endif /* USERSPACE_H_ */
