/*
 * ===============================================================================
 * (c) HTL Leonding
 * ALL RIGHTS RESERVED.
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * This document contains information belonging to HTL Leonding
 * No part of this document or information contained herein may be copied,
 * provided, used, or communicated about its content to any third party
 * in any form or by any means without explicit written permission by
 * HTL Leonding
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * All trademarks used in this document are property of their respective owners.
 * ===============================================================================
 */

#pragma once

template<typename C>
class Singleton {
	public:
		virtual ~Singleton() {
		}
		static C* GetInstance() {
			static SingletonGuard guard;
			if (0 == mMe) {
				mMe = new C();
			}
			return mMe;
		}

	protected:
		/** Singleton constructor */
		Singleton() {
		}

		static bool HasMe() {
			return mMe != 0;
		}

		static C* GetMe() {
			return mMe;
		}

		static void ApplyMe(C* me) {
			mMe = me;
		}

		/** Singleton memory guard */
		class SingletonGuard {
			public:
				~SingletonGuard() {
					if (0 != mMe) {
						delete mMe;
						mMe = 0;
					}
				}
		};

	private:
		friend class SingletonGuard;

		/** Singleton instance */
		static C* mMe;
		/** Singleton copy constructor */
		Singleton(const Singleton&);
};

template<typename C> C* Singleton<C>::mMe = 0;
