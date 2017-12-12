<template>
  <div class="portionSelectorOuter">
    <div :class="{arr:true, upArr:true}">
      <icon name="angle-up" scale="4"></icon>
      <span class="text">Back</span></div>
    <div class="portionSelector verticalCenter">
      <div :class="getClass()">-</div>
      <div>
        <h2>How many people are you cooking for?</h2>
        <p>
        <h1>{{showPortionCount}} <span v-if="showPortionCount > 1">people</span><span v-else>person</span></h1>
        </p>
      </div>
      <div class="rightArr arr">+</div>
    </div>
    <div :class="{arr:true, downArr:true}"><span class="text">Continue</span>
      <icon name="angle-down" scale="4"></icon>
    </div>
  </div>
</template>

<script>
  export default {
    name: 'portionSelector',
    props: ['eventBus', 'portionCount'],
    created() {
      this.eventBus.$on("pressed", this.handlePress);
      this.showPortionCount = this.portionCount;
    },
    data() {
      return {
        showPortionCount: 0,
        focussed: true
      }
    },
    methods: {
      handlePress(dir) {
        if (this.focussed) {
          if (dir === "left" && this.showPortionCount > 1) {
            this.showPortionCount--;
            this.eventBus.$emit("portionCount", this.showPortionCount)
          } else if (dir === "right") {
            this.showPortionCount++;
            this.eventBus.$emit("portionCount", this.showPortionCount)
          } else if (dir === "up") {
            this.focussed = false;
            this.$parent.upPressed();
          } else if (dir === "down") {
            this.focussed = false;
            this.$parent.downPressed();
          }
        }
      },

      getClass() {
        var str = "leftArr arr "
        if (this.showPortionCount <= 1) {
          str += "noshow"
        }
        return str;
      }
    }
  }
</script>


<style scoped>
  .portionSelector {
    display: flex;
    flex-direction: row;
    justify-content: space-between;
    padding: 10px;
  }

  .portionSelectorOuter {
    display: flex;
    flex-direction: column;
    justify-content: space-between;
    height: 100%;
  }

  .arr {
    font-size: 100px;
  }

  .leftArr {
    /*float:left;*/
    transition: 0.5s;
    padding-bottom: 11px;
  }

  .leftArr.noshow {
    opacity: 0;
    transition: 0.5s;
  }

  .rightArr {
    /*float:right;*/
  }

  .arr.downArr, .arr.upArr {
    font-size: 20px;
  }

  .arr.downArr .text {
    display: block;
  }

  .arr.upArr .text {
    display: block;
  }

  /*&rsaquo;*/

</style>
