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

import { createRouter, createWebHistory } from 'vue-router'

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    {
      path: '/',
      name: 'home',
      component: () => import('../views/HomeView.vue')
    },
    {
      path: '/games',
      name: 'games',
      component: () => import('../views/Games/GamesView.vue'),
    },
    {
      path: "/games/shot-challenge",
      name: "shot-challenge",
      component: () => import('../views/Games/ShotChallengeView.vue')
    },
    {
      path: "/games/timed-shots-challenge",
      name: "timed_shots-challenge",
      component: () => import('../views/Games/TimedShotsChallengeView.vue')
    },
    {
      path: '/settings',
      name: 'settings',
      redirect: '/settings/audio',
      component: () => import('../views/Settings/SettingsView.vue'),
      children: [
        {
          path: "audio",
          name: "audio",
          component: () => import('../views/Settings/AudioSettingsView.vue')
        },
        {
          path: "devices",
          name: "devices",
          component: () => import('../views/Settings/DevicesSettingsView.vue')
        },
        
        {
          path: "system",
          name: "system",
          component: () => import('../views/Settings/SystemSettingsView.vue')
        },
      ]
    },
    {
      path: '/about',
      name: 'about',
      component: () => import('../views/AboutView.vue')
    },
      //404 not found
    {
      path: '/:pathMatch(.*)*',
      name: 'not-found',
      component: () => import('../views/NotFoundView.vue')
    }
    /*{
      path: '/about',
      name: 'about',
      // route level code-splitting
      // this generates a separate chunk (About.[hash].js) for this route
      // which is lazy-loaded when the route is visited.
      //component: () => import('../views/AboutView.vue')
    }*/
  ]
})

export default router
