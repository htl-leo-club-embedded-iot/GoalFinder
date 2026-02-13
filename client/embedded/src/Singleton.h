/*
 * ===============================================================================
 * (c) HTBLA Leonding 2024 - 2026
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Licensed under MIT License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the license.
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
