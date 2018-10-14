import React, { Component } from 'react';

export default class MyImages extends Component {
	render() {
		return (
			<section className="main-section loading">
				<div className="gallery">
					{this.props.gallery.map((e, i) => {
						return <div key={i} style={{backgroundImage: "url(/img/"+e.url+")"}}>
							<div>
								<span><i>{e.views}</i> views</span>
							</div>
						</div>;
					})}
				</div>
			</section>
		);
	}
}