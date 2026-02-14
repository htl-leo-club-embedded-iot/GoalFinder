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

import { computed, type WritableComputedRef } from 'vue';

export function useClampedValue(
  getter: () => number,
  setter: (value: number) => void,
  min: number,
  max: number
): WritableComputedRef<number> {
  return computed({
    get: getter,
    set: (value: number) => {
      setter(Math.max(min, Math.min(max, value)));
    }
  });
}
