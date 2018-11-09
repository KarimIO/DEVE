import React, { Component } from 'react';

export default class ImageView extends Component {
	render() {
		return (
			<div className={`image-view${this.props.shown ? "" : " hide"}`} style={{backgroundImage: "url(data:image/png;base64," + this.props.image + ")"}}>
                <div onClick={this.props.hideImageView}></div>
            </div>
		);
	}
}