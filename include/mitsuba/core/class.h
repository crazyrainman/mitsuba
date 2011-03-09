/*
    This file is part of Mitsuba, a physically based rendering system.

    Copyright (c) 2007-2010 by Wenzel Jakob and others.

    Mitsuba is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License Version 3
    as published by the Free Software Foundation.

    Mitsuba is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#if !defined(__CLASS_H)
#define __CLASS_H

MTS_NAMESPACE_BEGIN

/**
 * \headerfile mitsuba/core/class.h mitsuba/mitsuba.h
 * \brief Stores meta-information about \ref Object instances.
 *
 * This class provides a thin layer of RTTI (run-time type information),
 * which is useful for doing things like:
 *
 * <ul>
 *    <li> Checking if an object derives from a certain class </li>
 *    <li> Determining the parent of a class at runtime </li>
 *    <li> Instantiating a class by name </li>
 *    <li> Unserializing a class from a binary data stream </li>
 * </ul>
 *
 * \sa ref, Object
 * \ingroup libcore
 */
class MTS_EXPORT_CORE Class {
public:
	/**
	 * \brief Construct a new class descriptor
	 *
	 * This method should never be called manually. Instead, use
	 * one of the  \ref MTS_IMPLEMENT_CLASS, \ref MTS_IMPLEMENT_CLASS_S,
	 * \ref MTS_IMPLEMENT_CLASS_I or \ref MTS_IMPLEMENT_CLASS_IS macros
	 * to automatically do this for you.
	 *
	 * \param name Name of the class
	 * \param abstract \a true if the class contains pure virtual methods
	 * \param superClassName Name of the parent class
	 * \param instPtr Pointer to an instantiation function
	 * \param unSerPtr Pointer to an unserialization function
	 */
	Class(const std::string &name, bool abstract, const std::string &superClassName, 
		void *instPtr = NULL, void *unSerPtr = NULL);

	/// Return the name of the represented class
	inline const std::string &getName() const { return m_name; }

	/**
	 * \brief Return whether or not the class represented
	 * by this Class object contains pure virtual methods
	 */
	inline bool isAbstract() const { return m_abstract; }

	/// Does the class support instantiation over RTTI?
	inline bool isInstantiable() const { return m_instPtr != NULL; }

	/// Does the class support serialization?
	inline bool isSerializable() const { return m_unSerPtr != NULL; }

	/** \brief Return the Class object associated with the parent
	 * class of NULL if it does not have one.
	 */
	inline const Class *getSuperClass() const { return m_superClass; }

	/// Check whether this class derives from \a theClass
	bool derivesFrom(const Class *theClass) const;

	/// Look up a class by its name
	static const Class *forName(const std::string &name);

	/** \brief Look up a class by its name. Avoids allocating
	 * heap space by taking a character array as parameter
	 */
	static const Class *forName(const char *name);

	/** \brief Unserialize an instance of the class (if this is
	 * supported).
	 */
	Object *unserialize(Stream *stream = NULL, InstanceManager *manager = NULL) const;

	/// Generate an instance of this class (if this is supported)
	Object *instantiate() const;

	/** \brief Initializes the built-in RTTI and creates
	 * a list of all compiled classes
	 */
	static void staticInitialization();

	/// Free the memory taken by staticInitialization()
	static void staticShutdown();
private:
	/** \brief Initialize a class - called by
	 * staticInitialization()
	 */
	static void initializeOnce(Class *theClass);
private:
	std::string m_name;
	bool m_abstract;
	Class *m_superClass;
	std::string m_superClassName;
	void *m_instPtr, *m_unSerPtr;
};

/*! \addtogroup libcore */

/*! @{ */

#define MTS_CLASS(x) x::m_theClass

/**
 * \brief This macro must be used in the declaration of 
 * all classes derived from \ref Object.
 */
#define MTS_DECLARE_CLASS() \
	virtual const Class *getClass() const; \
public: \
	static Class *m_theClass; 

/**
 * \brief Creates basic RTTI support for a class
 * \param name Name of the class
 * \param abstract \a true if the class contains pure virtual methods
 * \param super Name of the parent class
 */
#define MTS_IMPLEMENT_CLASS(name, abstract, super) \
	Class *name::m_theClass = new Class(#name, abstract, #super); \
	const Class *name::getClass() const { \
		return m_theClass;\
	}

/**
 * \brief Creates basic RTTI support for a class. Assumes that
 * the class can be instantiated by name.
 * \param name Name of the class
 * \param abstract \a true if the class contains pure virtual methods
 * \param super Name of the parent class
 */
#define MTS_IMPLEMENT_CLASS_I(name, abstract, super) \
	Object *__##name ##_inst() { \
		return new name(); \
	} \
	Class *name::m_theClass = new Class(#name, abstract, #super, (void *) &__##name ##_inst, NULL); \
	const Class *name::getClass() const { \
		return m_theClass;\
	}

/**
 * \brief Creates basic RTTI support for a class. Assumes that
 * the class can be unserialized from a binary data stream.
 * \param name Name of the class
 * \param abstract \a true if the class contains pure virtual methods
 * \param super Name of the parent class
 */
#define MTS_IMPLEMENT_CLASS_S(name, abstract, super) \
	Object *__##name ##_unSer(Stream *stream, InstanceManager *manager) { \
		return new name(stream, manager); \
	} \
	Class *name::m_theClass = new Class(#name, abstract, #super, NULL, (void *) &__##name ##_unSer); \
	const Class *name::getClass() const { \
		return m_theClass;\
	}

/**
 * \brief Creates basic RTTI support for a class. Assumes that
 * the class can be unserialized from a binary data stream as well
 * as instantiated by name.
 * \param name Name of the class
 * \param abstract \a true if the class contains pure virtual methods
 * \param super Name of the parent class
 */
#define MTS_IMPLEMENT_CLASS_IS(name, abstract, super) \
	Object *__##name ##_unSer(Stream *stream, InstanceManager *manager) { \
		return new name(stream, manager); \
	} \
	Object *__##name ##_inst() { \
		return new name(); \
	} \
	Class *name::m_theClass = new Class(#name, abstract, #super, (void *) &__##name ##_inst, (void *) &__##name ##_unSer); \
	const Class *name::getClass() const { \
		return m_theClass;\
	}

/*! @} */

MTS_NAMESPACE_END

#endif /* __CLASS_H */

